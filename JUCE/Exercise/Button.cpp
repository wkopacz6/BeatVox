/*
  ==============================================================================

    Button.cpp
    Created: 21 Sep 2020 11:30:46pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Button.h"

//==============================================================================
Button::Button()
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(button1);
    setSize(300, 200);
}

Button::~Button()
{
    setLookAndFeel(nullptr);
}

void Button::paint (juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::red);
    g.fillRect(getWidth() / 4, getHeight() / 4, getWidth() / 2, getHeight() / 2);
    
    g.setColour (juce::Colours::white);
    g.setFont (26.0f);
    g.drawText ("BeatVOX", getWidth()/4, 20, getWidth()/2, 40,
                juce::Justification::centred, true);   // draw some placeholder text
}

void Button::resized()
{
    auto border = 4;
    auto area = getLocalBounds();

    auto buttonHeight = 30;

    button1.setBounds(area.removeFromTop(buttonHeight).reduced(border));

}
