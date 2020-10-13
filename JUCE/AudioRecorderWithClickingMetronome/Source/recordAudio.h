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

class recordAudio : public juce::AudioAppComponent
{
public:
    recordAudio();
    ~recordAudio();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    void startRecording();
    void stopRecording();
    void createBuffer(int numBars, int bpm);
    void resetRecording();
    void metEnabled(bool enable);
    void tester();

    bool isRecording{ false };
  
private:

    double sampleRate{ 0.0 };
    int startSample{ 0 };
 
    int numInputChannels{ 2 };
    double mSampleRate{ 0 };
    int mBufferSize{ 0 };
    juce::AudioBuffer<float> bufferRecordedAudio;
    Metronome metronome;
    

    
};
