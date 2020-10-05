#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(buttonRecord);
    buttonRecord.setEnabled(true);
    buttonRecord.onClick = [this]() { start(); };
    
    addAndMakeVisible(buttonReset);
    buttonReset.setEnabled(false);
    buttonReset.onClick = [this]() { reset(); };

    deviceManager.initialise(2, 2, nullptr, true, {}, nullptr);
    deviceManager.addAudioCallback (&recorder);

    setSize (200, 200);
}

MainComponent::~MainComponent()
{
    deviceManager.removeAudioCallback(&recorder);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();

    juce::FlexBox flexBox;
    flexBox.items.add(juce::FlexItem(100, 100, buttonRecord));
    flexBox.items.add(juce::FlexItem(100, 100, buttonReset));
    flexBox.performLayout(bounds);
}

void MainComponent::start()
{
    recorder.startRecording();

    buttonRecord.setEnabled(false);
    buttonReset.setEnabled(true);
}

void MainComponent::reset()
{
    recorder.resetRecording();

    buttonRecord.setEnabled(true);
    buttonReset.setEnabled(false);
}



