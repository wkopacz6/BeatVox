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
    
    void metPressed();
    void ioPressed();
    void done();

    void timerCallback();
private:
    bool metEnable{ false };

    recordAudio recorder;

    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonStop{ "Stop Recording" };
    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    juce::TextButton buttonMet{ "Metronome Off" };
    juce::TextButton buttonIO{ "Open IO Setup" };

    //number of bars for user to record
    juce::Slider barCount;
    juce::Label numberOfBarsLabel;
    
    //bpm slider
    juce::Slider newBpm;
    juce::Label newBpmLabel;

    void start();
    void reset();
    void stop();
    void sendBufferVals();
    void dumpDataToCSV();
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

