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
    std::unique_ptr<juce::DialogWindow> menu;
    bool metEnable{ false };

    recordAudio recorder;
    buildMIDI midi;

    juce::TextButton buttonRecord{ "Record" };
    juce::TextButton buttonReset{ "Reset" };
    juce::TextButton buttonStop{ "Stop Recording" };
    juce::TextButton buttonDump{ "Dump to\n .csv file" };
    juce::TextButton buttonMet{ "Metronome Off" };
    juce::TextButton buttonMidiTest{ "Test MIDI" };
    juce::TextButton audioSetupButton{ "IO setup" };
    juce::TextButton buttonMidiTest1{ "Play" };

    juce::TextEditor errorBox;
    juce::TextEditor outputBox;
    juce::ComboBox midiBox;

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
    void midiBoxChanged();
    void buttonMidiTestPressed();
    void buttonMidiTestPressed1();
    void ioSetup();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

