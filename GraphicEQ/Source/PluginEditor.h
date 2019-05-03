#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GUI Extras/ChowSlider.h"
#include "GUI Extras/MyLNF.h"

using namespace Constants;

//==============================================================================
/**
*/
class GraphicEqAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener
{
public:
    GraphicEqAudioProcessorEditor (GraphicEqAudioProcessor&);
    ~GraphicEqAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    static void createSlider (ChowSlider& slide, AudioParameterFloat* param, LookAndFeel& lnf,
        Component* comp, String suffix = String(), float step = 0.1f);
    static void createButton (TextButton& button, AudioParameterBool* param,
                            LookAndFeel& lnf, Component* comp, std::function<void()> onClick = {});

    static void createBox (ComboBox& box, StringArray choices, Component* comp,
                        int firstChoice = 0, std::function<void()> onChange = {}, int idOffset = 1);

    static AudioParameterFloat* getParamForSlider (Slider* slider, GraphicEqAudioProcessor& proc);

    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;

private:
    GraphicEqAudioProcessor& processor;

    MyLNF myLNF;

    ChowSlider sliders[numBands];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEqAudioProcessorEditor)
};
