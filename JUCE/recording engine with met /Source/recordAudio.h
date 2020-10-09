/*
  ==============================================================================

    recordAudio.h
    Created: 2 Oct 2020 12:47:40pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
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
    void createBuffer(int samplesAllocated);
    void resetRecording();
    void barChanger(double newBars);
    void bpmChanger(double bpmUpdate);
    void tester();

    bool isRecording{ false };
  
private:
    int mSampleInterval { 0 };
    int mTotalSamples { 0 };
    double mSampleRate { 0 };
    int mBpm { 60 };
    int mSamplesToGo { 0 };
    double sampleRate{ 44100.0 };
    int startSample{ 0 };
    
    int numberOfBars{ 1 };
    int samplesToAllocate{ 0 };

    juce::AudioBuffer<float> bufferRecordedAudio;
    

    
};
