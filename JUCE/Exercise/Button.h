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
class Button  : public juce::Component
{
public:
    Button();
    ~Button() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String currentSizeAsString;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Button)
};
