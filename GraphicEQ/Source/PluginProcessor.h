#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterBank.h"

namespace Constants
{
    enum
    {
        numBands = 10,
    };
}

using namespace Constants;
//==============================================================================
/**
*/
class GraphicEqAudioProcessor : public AudioProcessor,
                                private AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    GraphicEqAudioProcessor();
    ~GraphicEqAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterValueChanged (int paramIndex, float /*newValue*/) override;
    void parameterGestureChanged (int /*paramIndex*/, bool /*gestureIsStarting*/) override {}

    AudioParameterFloat* gainDB[numBands];

private:
    Filterbank filterbanks[2];

    const String names[numBands] = {"30", "60", "120", "250", "500", "1k", "2k", "4k", "8k", "16k"};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqAudioProcessor)
};
