#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GUI Extras/ChowSlider.h"
#include "GUI Extras/MyLNF.h"

//==============================================================================
/**
*/
class Noise_suppressionAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener
{
public:
    Noise_suppressionAudioProcessorEditor (Noise_suppressionAudioProcessor&);
    ~Noise_suppressionAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    static void createSlider (ChowSlider& slide, AudioParameterFloat* param, LookAndFeel& lnf,
                              Component* comp, String suffix = String(), float skew = 0.0f, float step = 0.1f);
    static void createButton (TextButton& button, AudioParameterBool* param,
        LookAndFeel& lnf, Component* comp, std::function<void()> onClick = {});

    static void createBox (ComboBox& box, StringArray choices, Component* comp,
        int firstChoice = 0, std::function<void()> onChange = {}, int idOffset = 1);

    static AudioParameterFloat* getParamForSlider (Slider* slider, Noise_suppressionAudioProcessor& proc);

    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;

private:
    Noise_suppressionAudioProcessor& processor;
    MyLNF myLNF;

    float getBinPower();

    ChowSlider threshSlide;
    ChowSlider speedSlide;
    ChowSlider binSlide;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Noise_suppressionAudioProcessorEditor)
};
