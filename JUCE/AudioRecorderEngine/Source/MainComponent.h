#pragma once

#include <JuceHeader.h>
#include "recordAudio.h"

class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
   

private:
    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::AudioDeviceManager deviceManager;
    recordAudio recorder;
    
    void start();
    void reset();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};