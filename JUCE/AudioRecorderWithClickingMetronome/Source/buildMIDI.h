/*
  ==============================================================================

    buildMIDI.h
    Created: 18 Oct 2020 6:04:23pm
    Author:  JohnK

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class buildMIDI : public juce::MidiMessage
{
public:
    buildMIDI();
    ~buildMIDI();
    void createMessage();
    void addMessageToBuffer();
    void outputMIDI();
private:
    juce::MidiBuffer midiBuffer;
};
