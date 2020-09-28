#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //initializing enum
    enum transportState {
        Stopped,
        Starting,
        Stopping,
        Playing
    };
    transportState state;

    //additionally added functions
    void buttonFileClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void transportStateChanged(transportState newState);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //initializing variables from various JUCE classes
    juce::TextButton buttonFile;
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transport;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    std::unique_ptr<juce::AudioFormatReaderSource> playSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
