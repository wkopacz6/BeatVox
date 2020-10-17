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
    void createBuffer(int numBars, double bpm);
    void resetRecording();
    void metEnabled(bool enable);
    void tester();

    juce::AudioBuffer<float> bufferRecordedAudio;
    bool isRecording{ false };
    juce::String deviceName{ "" };


private:
    double mBpm { 120 };
    double mSampleRate{ 0 };
    int mBar { 4 };
    int startSample{ 0 };
    int numInputChannels{ 2 };
    int mBufferSize{ 0 };

    
    Metronome metronome;
   
};