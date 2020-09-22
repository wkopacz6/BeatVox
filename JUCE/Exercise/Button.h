/*
  ==============================================================================

    Button.h
    Created: 21 Sep 2020 11:30:46pm
    Author:  JohnK

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel() {}
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        auto edge = 4;

        buttonArea.removeFromLeft(edge);
        buttonArea.removeFromTop(edge);

        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        g.fillRect(buttonArea);

        auto offset = isButtonDown ? -edge / 2 : -edge;
        buttonArea.translate(offset, offset);

        if (isMouseOverButton == true) {
            g.setColour(juce::Colours::red);
        }
        else {
            g.setColour(backgroundColour);
        }
        g.fillRect(buttonArea);
    }
};

class Button  : public juce::Component
{
public:
    Button();
    ~Button() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LookAndFeel lookAndFeel;
    juce::TextButton button1;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Button)
};
