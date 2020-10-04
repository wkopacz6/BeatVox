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
    juce::AudioDeviceManager deviceManager;
    recordAudio recorder;
    
    void start();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};