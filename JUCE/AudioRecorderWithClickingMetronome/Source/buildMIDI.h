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
    void setDevice(int selected);
    void setSampleRate(double sampleRate);
    void fillBuffer(juce::Array<int> sampleArray, juce::Array<int> drumArray, juce::Array<int> velocityArray);
    void outputMIDI();

    
    juce::Array<juce::MidiDeviceInfo> midiList;
private:
    juce::MidiBuffer midiBuffer;
    juce::MidiOutput* midiOutput{ nullptr };
    juce::MidiDeviceInfo output;
    std::unique_ptr <juce::MidiOutput> out{ nullptr }; 
    double mSampleRate{ 48000 };
};
