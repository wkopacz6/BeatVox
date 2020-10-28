/*
  ==============================================================================

    recordAudio.h
    Created: 2 Oct 2020 12:47:40pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Metronome.h"
#include "buildMIDI.h"
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
    void fillMidiBuffer(juce::Array<int> onsetArray, juce::Array<int> drumArray, juce::Array<int> velocityArray);
    void outputMidi();

    juce::AudioBuffer<float> bufferRecordedAudio;
    bool isRecording{ false };
    bool errored{ false };
    juce::String deviceName{ "" };
    juce::AudioDeviceManager::AudioDeviceSetup deviceSetup;
    double mSampleRate{ 0.0 };
 
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetupComp;
    juce::MidiOutput* midiOutput{ nullptr };
private:
    
    juce::MidiBuffer midiBuffer;
    double mBpm { 120.0 };
    int mBar { 4 };
    int startSample{ 0 };
    int numInputChannels{ 1 };
    int numOutputChannels{ 2 };
    int mBufferSize{ 0 };

    Metronome metronome;
    buildMIDI midi;
    
};