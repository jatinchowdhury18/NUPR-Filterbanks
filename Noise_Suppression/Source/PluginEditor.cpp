#include "PluginProcessor.h"
#include "PluginEditor.h"

enum
{
    sliderWidth = 100,
    titleHeight = 30,
    numberHeight = 20,
};

//==============================================================================
Noise_suppressionAudioProcessorEditor::Noise_suppressionAudioProcessorEditor (Noise_suppressionAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (sliderWidth * 3, titleHeight + numberHeight + sliderWidth);

    createSlider (threshSlide, p.threshDB, myLNF, this, " dB");
    createSlider (speedSlide, p.speedMs, myLNF, this, " Ms", 100.0f);
    createSlider (binSlide, nullptr, myLNF, this, {}, 0.0f, 1.0f);
    binSlide.setRange (1, 10,  1);
    binSlide.setValue (1);
}

Noise_suppressionAudioProcessorEditor::~Noise_suppressionAudioProcessorEditor()
{
}

//==============================================================================
float Noise_suppressionAudioProcessorEditor::getBinPower()
{
    return Decibels::gainToDecibels (processor.getCellPower (int (binSlide.getValue())), -80.0f);
}

void Noise_suppressionAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::antiquewhite);

    g.setColour (Colours::blue);
    g.setFont (Font (20.0f).boldened());
    g.drawFittedText ("Noise Supression", getLocalBounds().removeFromTop (titleHeight), Justification::centred, 1);

    g.setColour (Colours::red);
    g.setFont (Font (18.0f).boldened());
    g.drawFittedText (threshSlide.getName(), 0, titleHeight, sliderWidth,
                      numberHeight, Justification::centred, 1);
    g.drawFittedText (speedSlide.getName(), sliderWidth, titleHeight, sliderWidth,
                      numberHeight, Justification::centred, 1);

    g.drawFittedText (("Pwr: " + String (getBinPower(), 1) + " dB"),
                      sliderWidth*2, titleHeight, sliderWidth, numberHeight, Justification::centred, 1);
}

void Noise_suppressionAudioProcessorEditor::resized()
{
    threshSlide.setBounds (0, titleHeight+numberHeight, sliderWidth, sliderWidth);
    speedSlide.setBounds (sliderWidth, titleHeight+numberHeight, sliderWidth, sliderWidth);
    binSlide.setBounds (sliderWidth*2, titleHeight+numberHeight, sliderWidth, sliderWidth);
}

void Noise_suppressionAudioProcessorEditor::createSlider (ChowSlider& slide, AudioParameterFloat* param, LookAndFeel& lnf,
    Component* comp, String suffix, float skew, float step)
{
    if (param != nullptr)
    {
        slide.setName(param->name);
        slide.setRange(param->range.start, param->range.end, step);
        slide.setDefaultValue (param->convertFrom0to1 (dynamic_cast<AudioProcessorParameterWithID*> (param)->getDefaultValue()));
        slide.setValue(*param);
    }

    slide.setLookAndFeel (&lnf);
    slide.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    slide.setColour (Slider::rotarySliderFillColourId, Colours::dodgerblue);
    slide.setColour (Slider::rotarySliderOutlineColourId, Colours::darkred);
    slide.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    slide.setColour (Slider::thumbColourId, Colours::darkred);
    slide.setColour (Slider::textBoxTextColourId, Colours::darkred);
    slide.setColour (Slider::textBoxHighlightColourId, Colours::grey);

    slide.setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    if (suffix.isNotEmpty())
        slide.setTextValueSuffix (" " + suffix);

    if (skew != 0)
        slide.setSkewFactorFromMidPoint (double (skew));

    slide.addListener (dynamic_cast<Slider::Listener*> (comp));
    comp->addAndMakeVisible (slide);
}

void Noise_suppressionAudioProcessorEditor::createButton (TextButton& button, AudioParameterBool* param,
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

void Noise_suppressionAudioProcessorEditor::createBox (ComboBox& box, StringArray choices, Component* comp,
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

AudioParameterFloat* Noise_suppressionAudioProcessorEditor::getParamForSlider (Slider* slider, Noise_suppressionAudioProcessor& proc)
{
    if (proc.threshDB->name == slider->getName())
       return proc.threshDB;
    else if (proc.speedMs->name == slider->getName())
        return proc.speedMs;

    return nullptr;
}

void Noise_suppressionAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (AudioParameterFloat* param = Noise_suppressionAudioProcessorEditor::getParamForSlider (slider, processor))
        *param = (float) slider->getValue();
}

void Noise_suppressionAudioProcessorEditor::sliderDragStarted(Slider* slider)
{
    if (AudioParameterFloat* param = Noise_suppressionAudioProcessorEditor::getParamForSlider (slider, processor))
        param->beginChangeGesture();
}

void Noise_suppressionAudioProcessorEditor::sliderDragEnded(Slider* slider)
{
    if (AudioParameterFloat* param = Noise_suppressionAudioProcessorEditor::getParamForSlider (slider, processor))
        param->endChangeGesture();
}
