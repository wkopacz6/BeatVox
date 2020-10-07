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

    addAndMakeVisible(buttonDump);
    buttonDump.setEnabled(false);
    buttonDump.onClick = [this]() { dumpDataToCSV(); };

    deviceManager.initialise(2, 2, nullptr, true, {}, nullptr);
    deviceManager.addAudioCallback (&recorder);
    
    setSize (300, 300);
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
    juce::Rectangle<int> bounds = getLocalBounds().reduced(5);

    juce::FlexBox flexBox;
    flexBox.items.add(juce::FlexItem(bounds.getWidth()/3, bounds.getHeight()/3, buttonRecord));
    flexBox.items.add(juce::FlexItem(bounds.getWidth() / 3, bounds.getHeight() / 3, buttonReset));
    flexBox.items.add(juce::FlexItem(bounds.getWidth() / 3, bounds.getHeight() / 3, buttonDump));
    flexBox.performLayout(bounds);
}


void MainComponent::start()
{

    recorder.startRecording();
    startTimer(500);

    buttonRecord.setButtonText("Recording...");
    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    buttonRecord.setEnabled(false);
    buttonReset.setEnabled(true);
    
}

void MainComponent::reset()
{
    recorder.resetRecording();
    stopTimer();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(true);
    buttonReset.setEnabled(false);
    buttonDump.setEnabled(false);
}

void MainComponent::dumpDataToCSV()
{
    recorder.tester();
}

void MainComponent::done()
{
    stopTimer();

    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    buttonRecord.setButtonText("Done!");

    buttonDump.setEnabled(true);
}

void MainComponent::timerCallback()
{
    if (!recorder.isRecording)
    {
        done();
    }
}



