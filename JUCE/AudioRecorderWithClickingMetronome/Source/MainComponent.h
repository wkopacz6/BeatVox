#pragma once

#include <JuceHeader.h>
#include "recordAudio.h"


class MainComponent : public juce::Component,
                      public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback(); 
    void metPressed();
    void done();
private:
    int beat = 1;
    bool metEnable{ false };

    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonStop{ "Stop Recording" };
    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    juce::TextButton buttonMet{ "Metronome Off" };

    //number of bars for user to record
    juce::Slider barCount;
    juce::Label numberOfBarsLabel;
    juce::AudioDeviceManager deviceManager;
    
    //bpm slider
    juce::Slider newBpm;
    juce::Label newBpmLabel;

    recordAudio recorder;
    
    //MetState metState { MetState::MetStopped };
    void start();
    void reset();
    void stop();
    void sendBufferVals();

    void dumpDataToCSV();
    
    
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

