#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(buttonRecord);
    buttonRecord.onClick = [this]() { start(); };
    
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
    auto area = getLocalBounds().reduced(5);

    buttonRecord.setBounds(area);

}

void MainComponent::start()
{
    recorder.startRecording();
}

