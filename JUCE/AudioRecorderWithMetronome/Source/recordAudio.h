/*
  ==============================================================================

    recordAudio.h
    Created: 2 Oct 2020 12:47:40pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Metronome.h"
#pragma once

class recordAudio : public juce::AudioIODeviceCallback
{
public:
    recordAudio();
    ~recordAudio();
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
        float** outputChannelData, int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void startRecording();
    void stopRecording();
    void createBuffer(int numBars, int bpm);
    void resetRecording();
    void tester();

    bool isRecording{ false };
  
private:

    double sampleRate{ 0.0 };
    int startSample{ 0 };
 

    juce::AudioBuffer<float> bufferRecordedAudio;
    Metronome metronome;
    

    
};
