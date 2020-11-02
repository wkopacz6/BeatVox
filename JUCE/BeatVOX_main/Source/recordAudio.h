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
    void createAudioBuffer(int numBars, double bpm);
    void resetRecording();
    void metEnabled(bool enable);
    void tester();
    void fillMidiBuffer(juce::Array<int> onsetArray, juce::Array<int> drumArray, juce::Array<int> velocityArray);
    void outputMidi();

    bool isRecording{ false };
    bool errored{ false };
 
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetupComp;
    juce::AudioBuffer<float> bufferRecordedAudio;
    double mSampleRate{ 0.0 };

private:
    
    double mBpm          { 120.0 };
    int mBar             { 4 };
    int startSample      { 0 };
    int numInputChannels { 1 };
    int numOutputChannels{ 2 };
    int mBufferSize      { 0 };

    juce::MidiBuffer bufferMidi;
    juce::MidiOutput* midiOutput{ nullptr };

    Metronome metronome;
    
};
