#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Noise_suppressionAudioProcessor::Noise_suppressionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (threshDB = new AudioParameterFloat ("threshDB", "Thresh", -80.0f, 0.0f, 0.0f));
    addParameter (speedMs = new AudioParameterFloat ("speedMs", "Speed", 1.0f, 1000.0f, 100.0f));

    threshDB->addListener (this);
    speedMs->addListener (this);

    filterbanks[0].setThresh (*threshDB);
    filterbanks[1].setThresh (*threshDB);
    filterbanks[0].setSpeed (*speedMs);
    filterbanks[1].setSpeed (*speedMs);
}

Noise_suppressionAudioProcessor::~Noise_suppressionAudioProcessor()
{
}

void Noise_suppressionAudioProcessor::parameterValueChanged (int paramIndex, float /*newValue*/)
{
    if (paramIndex == threshDB->getParameterIndex())
    {
        filterbanks[0].setThresh (*threshDB);
        filterbanks[1].setThresh (*threshDB);
    }
    else if (paramIndex == speedMs->getParameterIndex())
    {
        filterbanks[0].setSpeed (*speedMs);
        filterbanks[1].setSpeed (*speedMs);
    }
}

//==============================================================================
const String Noise_suppressionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Noise_suppressionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Noise_suppressionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Noise_suppressionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Noise_suppressionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Noise_suppressionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Noise_suppressionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Noise_suppressionAudioProcessor::setCurrentProgram (int index)
{
}

const String Noise_suppressionAudioProcessor::getProgramName (int index)
{
    return {};
}

void Noise_suppressionAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Noise_suppressionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    filterbanks[0].reset (sampleRate, samplesPerBlock);
    filterbanks[1].reset (sampleRate, samplesPerBlock);

    setLatencySamples (2048);
}

void Noise_suppressionAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Noise_suppressionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Noise_suppressionAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        filterbanks[channel].process (channelData, buffer.getNumSamples());
    }

    auto editor = getActiveEditor();
    if (editor != nullptr)
        MessageManager::callAsync ([editor] { editor->repaint(); });
}

float Noise_suppressionAudioProcessor::getCellPower (int cellIndex) const
{
    return (filterbanks[0].getCellPower (cellIndex) + filterbanks[1].getCellPower (cellIndex) / 2);
}

//==============================================================================
bool Noise_suppressionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Noise_suppressionAudioProcessor::createEditor()
{
    return new Noise_suppressionAudioProcessorEditor (*this);
}

//==============================================================================
void Noise_suppressionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Noise_suppressionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Noise_suppressionAudioProcessor();
}
