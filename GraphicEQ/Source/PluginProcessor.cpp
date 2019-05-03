#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicEqAudioProcessor::GraphicEqAudioProcessor()
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
    for (int i = 0; i < numBands; i++)
    {
        addParameter (gainDB[i] = new AudioParameterFloat ("gainDB" + String(i), names[i], -12.0f, 12.0f, 0.0f));
        gainDB[i]->addListener (this);

        filterbanks[0].setGain (*gainDB[i], i+1);
    }
}

GraphicEqAudioProcessor::~GraphicEqAudioProcessor()
{
}

void GraphicEqAudioProcessor::parameterValueChanged (int paramIndex, float /*newValue*/)
{
    for (int i = 0; i < numBands; i++)
    {
        if (paramIndex == gainDB[i]->getParameterIndex())
        {
            filterbanks[0].setGain (*gainDB[i], i+1);
            filterbanks[1].setGain (*gainDB[i], i+1);
        }
    }
}

//==============================================================================
const String GraphicEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GraphicEqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GraphicEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GraphicEqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GraphicEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GraphicEqAudioProcessor::setCurrentProgram (int index)
{
}

const String GraphicEqAudioProcessor::getProgramName (int index)
{
    return {};
}

void GraphicEqAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GraphicEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    filterbanks[0].reset();
    filterbanks[1].reset();
}

void GraphicEqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GraphicEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GraphicEqAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        filterbanks[channel].process (channelData, buffer.getNumSamples());
    }
}

//==============================================================================
bool GraphicEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GraphicEqAudioProcessor::createEditor()
{
    return new GraphicEqAudioProcessorEditor (*this);
}

//==============================================================================
void GraphicEqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GraphicEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphicEqAudioProcessor();
}
