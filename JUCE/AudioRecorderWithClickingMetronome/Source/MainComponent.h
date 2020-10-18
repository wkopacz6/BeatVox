#pragma once

#include <JuceHeader.h>
#include "recordAudio.h"
#include "buildMIDI.h"


class MainComponent : public juce::Component,
                    public juce::Timer, public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void metPressed();
    void done();
    void error();

    void timerCallback();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    bool metEnable{ false };

    recordAudio recorder;

    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonStop{ "Stop Recording" };
    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    juce::TextButton buttonMet{ "Metronome Off" };

    juce::TextEditor inputBox;
    juce::TextEditor outputBox;

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

