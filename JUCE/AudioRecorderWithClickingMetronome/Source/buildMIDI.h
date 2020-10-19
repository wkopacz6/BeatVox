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
    void prepareToPlay(juce::MidiOutput* output, double sampleRate);
    void fillBuffer(const int *sampleArray, const int *drumArray, const int *velocityArray);
    void outputMIDI();

    double mSampleRate;
private:
    juce::MidiBuffer midiBuffer;
    juce::MidiOutput* midiOutput{ nullptr };
};
