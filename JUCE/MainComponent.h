#pragma once

#include <JuceHeader.h>
#include "recordAudio.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void changeBars();
    void changeBpm();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    //------------------------------------------------------------------------------
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback(); 

    void done();
private:
    
    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonStop{ "Stop Recording" };
    

    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    
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
    
    void dumpDataToCSV();
    
    
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

