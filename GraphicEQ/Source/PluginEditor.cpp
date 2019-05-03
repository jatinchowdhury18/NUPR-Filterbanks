#include "PluginProcessor.h"
#include "PluginEditor.h"

enum
{
    sliderWidth = 100,
    titleHeight = 30,
    numberHeight = 20,
};

//==============================================================================
GraphicEqAudioProcessorEditor::GraphicEqAudioProcessorEditor (GraphicEqAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (sliderWidth * numBands, titleHeight + numberHeight + sliderWidth);

    for (int i = 0; i < numBands; i++)
        createSlider (sliders[i], p.gainDB[i], myLNF, this, " dB");
}

GraphicEqAudioProcessorEditor::~GraphicEqAudioProcessorEditor()
{
}

//==============================================================================
void GraphicEqAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::antiquewhite);

    g.setColour (Colours::blue);
    g.setFont (Font (20.0f).boldened());
    g.drawFittedText ("Graphic EQ", getLocalBounds().removeFromTop (titleHeight), Justification::centred, 1);

    g.setColour (Colours::red);
    g.setFont (Font (18.0f).boldened());
    for (int i = 0; i < numBands; i++)
        g.drawFittedText (sliders[i].getName(), sliderWidth * i, titleHeight, sliderWidth, numberHeight, Justification::centred, 1);
}

void GraphicEqAudioProcessorEditor::resized()
{
    for (int i = 0; i < numBands; i++)
        sliders[i].setBounds (sliderWidth * i, titleHeight+numberHeight, sliderWidth, sliderWidth);
}

void GraphicEqAudioProcessorEditor::createSlider (ChowSlider& slide, AudioParameterFloat* param, LookAndFeel& lnf,
    Component* comp, String suffix, float step)
{
    slide.setName(param->name);
    slide.setRange(param->range.start, param->range.end, step);
    slide.setDefaultValue (param->convertFrom0to1 (dynamic_cast<AudioProcessorParameterWithID*> (param)->getDefaultValue()));
    slide.setValue(*param);

    slide.setLookAndFeel (&lnf);
    slide.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slide.setColour (Slider::rotarySliderFillColourId, Colours::dodgerblue);
    slide.setColour (Slider::rotarySliderOutlineColourId, Colours::darkred);
    slide.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    slide.setColour (Slider::thumbColourId, Colours::darkred);
    slide.setColour (Slider::textBoxTextColourId, Colours::darkred);
    slide.setColour (Slider::textBoxHighlightColourId, Colours::grey);

    slide.setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    if (suffix.isNotEmpty())
        slide.setTextValueSuffix (" " + suffix);

    slide.addListener (dynamic_cast<Slider::Listener*> (comp));
    comp->addAndMakeVisible (slide);
}

void GraphicEqAudioProcessorEditor::createButton (TextButton& button, AudioParameterBool* param,
    LookAndFeel& lnf, Component* comp, std::function<void()> onClick)
{
    button.setName (param->name);
    button.setButtonText (button.getName());

    button.setLookAndFeel (&lnf);
    button.setColour (TextButton::buttonOnColourId, Colours::dodgerblue);
    button.setColour (TextButton::textColourOnId, Colours::darkred);
    button.setColour (TextButton::buttonColourId, Colours::transparentWhite);
    button.setColour (TextButton::textColourOffId, Colours::darkred);

    button.setToggleState (param->get(), dontSendNotification);
    button.setClickingTogglesState (true);

    comp->addAndMakeVisible (button);

    button.onClick = onClick;
}

void GraphicEqAudioProcessorEditor::createBox (ComboBox& box, StringArray choices, Component* comp,
    int firstChoice, std::function<void()> onChange, int idOffset)
{
    box.addItemList (choices, idOffset);
    box.setSelectedItemIndex (firstChoice, dontSendNotification);
    box.onChange = onChange;

    box.setColour (ComboBox::backgroundColourId, Colours::transparentWhite);
    box.setColour (ComboBox::outlineColourId, Colours::darkred);
    box.setColour (ComboBox::textColourId, Colours::darkred);
    box.setColour (ComboBox::arrowColourId, Colours::darkred);
    comp->getLookAndFeel().setColour (PopupMenu::backgroundColourId, Colours::slategrey);
    comp->getLookAndFeel().setColour (PopupMenu::textColourId, Colours::darkred);
    comp->getLookAndFeel().setColour (PopupMenu::highlightedBackgroundColourId, Colours::dodgerblue);
    comp->addAndMakeVisible (box);
}

AudioParameterFloat* GraphicEqAudioProcessorEditor::getParamForSlider (Slider* slider, GraphicEqAudioProcessor& proc)
{
    for (int i = 0; i < numBands; i++)
        if (proc.gainDB[i]->name == slider->getName())
            return proc.gainDB[i];
    
    return nullptr;
}

void GraphicEqAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (AudioParameterFloat* param = GraphicEqAudioProcessorEditor::getParamForSlider (slider, processor))
        *param = (float) slider->getValue();
}

void GraphicEqAudioProcessorEditor::sliderDragStarted(Slider* slider)
{
    if (AudioParameterFloat* param = GraphicEqAudioProcessorEditor::getParamForSlider (slider, processor))
        param->beginChangeGesture();
}

void GraphicEqAudioProcessorEditor::sliderDragEnded(Slider* slider)
{
    if (AudioParameterFloat* param = GraphicEqAudioProcessorEditor::getParamForSlider (slider, processor))
        param->endChangeGesture();
}
