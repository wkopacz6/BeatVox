#pragma once

#include <JuceHeader.h>
#include "recordAudio.h"


class MainComponent : public juce::Component, public juce::Timer

{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback();

private:
    
    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    juce::AudioDeviceManager deviceManager;

    recordAudio recorder;
    
    void start();
    void reset();
    void dumpDataToCSV();
    void done();
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

