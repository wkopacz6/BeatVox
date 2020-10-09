/*
  ==============================================================================

    Metronome.h
    Created: 9 Oct 2020 9:37:58am
    Author:  JohnK

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
class Metronome : public juce::Component
{
public:
    void countSamples(int bufferLength);
    void reset();
    void setInterval(int bpm, double sampleRate);
private:
    int mTotalSamples{ 0 };
    int mSampleInterval{ 0 };
    int beat{ 1 };
};