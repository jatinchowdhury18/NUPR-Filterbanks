#ifndef MYLNF_H_INCLUDED
#define MYLNF_H_INCLUDED

#include "JuceHeader.h"

class MyLNF : public LookAndFeel_V4
{
public:
    MyLNF()
    {
        setColour (ComboBox::outlineColourId, Colours::darkred);
    }

    Font getTextButtonFont (TextButton& /*button*/, int buttonHeight) override
    {
        return Font (buttonHeight * 0.75f).boldened();
    }

    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override
    {
        auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
        auto fill    = slider.isEnabled() ? slider.findColour (Slider::rotarySliderFillColourId) : Colours::grey;

        auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

        auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin (5.0f, radius * 0.1f);
        auto arcRadius = radius - lineW;

        g.setColour (outline);
        g.fillEllipse (Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (bounds.getCentre()));

        g.setColour (fill);
        g.fillEllipse (Rectangle<float> (arcRadius * 2.0f, arcRadius * 2.0f).withCentre (bounds.getCentre()));

        Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                                 bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));
        g.setColour (slider.findColour (Slider::thumbColourId));
        g.drawLine (bounds.getCentreX(), bounds.getCentreY(), thumbPoint.x, thumbPoint.y, lineW);
    }

    Slider::SliderLayout getSliderLayout (Slider& slider) override
    {
        auto layout = LookAndFeel_V4::getSliderLayout (slider);

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth  = jmax (0, jmin (slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
        auto textBoxHeight = jmax (0, jmin (slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth (textBoxWidth);
                layout.textBoxBounds.setHeight (textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)           layout.textBoxBounds.setX (0);
                else if (textBoxPos == Slider::TextBoxRight)     layout.textBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)          layout.textBoxBounds.setY (0);
                else if (textBoxPos == Slider::TextBoxBelow)     layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight - 10);
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        return layout;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyLNF)
};

#endif //MYLNF_H_INCLUDED
