#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    //initialize bar slider and bpm slider
    addAndMakeVisible (barCount);
    barCount.setRange(1, 16,1);
    barCount.setTextValueSuffix (" Bars");
    addAndMakeVisible (newBpm);
    newBpm.setRange(20, 200, 1);
    newBpm.setTextValueSuffix (" BPM");
    newBpm.setValue(120);
    barCount.setValue(4);
    
    
    //initialize bar slider label and bpm slider label
    addAndMakeVisible (numberOfBarsLabel);
    numberOfBarsLabel.setText ("Number of Bars", juce::dontSendNotification);
    numberOfBarsLabel.attachToComponent (&barCount, true);
    
    addAndMakeVisible (newBpmLabel);
    newBpmLabel.setText ("BPM", juce::dontSendNotification);
    newBpmLabel.attachToComponent (&newBpm, true);
    
    
    
    
    //initialize all buttons and sliders with lambdas
    //set value change detected from sliders
    
    barCount.onValueChange = [this]() { sendBufferVals(); };
    newBpm.onValueChange = [this]() { sendBufferVals(); };
    
    addAndMakeVisible(buttonRecord);
    buttonRecord.setEnabled(true);
    buttonRecord.onClick = [this]() { start(); };
    
    addAndMakeVisible(buttonReset);
    buttonReset.setEnabled(false);
    buttonReset.onClick = [this]() { reset(); };
    
    addAndMakeVisible(buttonStop);
    buttonStop.setEnabled(false);
    buttonStop.onClick = [this]() { stop(); };

    addAndMakeVisible(buttonDump);
    buttonDump.setEnabled(false);
    buttonDump.onClick = [this]() { dumpDataToCSV(); };

    deviceManager.initialise(2, 2, nullptr, true, {}, nullptr);
    deviceManager.addAudioCallback (&recorder);
    
    recorder.createBuffer(barCount.getValue(), newBpm.getValue());
    setSize (600, 600);

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
    buttonRecord.setBounds(10, 10, getWidth()-20, 30);
    buttonStop.setBounds(10, 50, getWidth() - 20, 30);
    buttonReset.setBounds(10, 90, getWidth() - 20, 30);
    buttonDump.setBounds(10, 130, getWidth() - 20, 30);
    barCount.setBounds(10, 170, getWidth() - 20, 30);
    newBpm.setBounds(10, 210, getWidth() - 20, 30);
}



void MainComponent::start()
{
    
    recorder.startRecording();
 
    startTimer( ((6000/newBpm.getValue()) * 4 * barCount.getValue()) + 10);
    buttonRecord.setButtonText("Recording...");
    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    buttonRecord.setEnabled(false);
    buttonReset.setEnabled(false);
    buttonStop.setEnabled(true);
    barCount.setEnabled(false);
    newBpm.setEnabled(false);

    
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
    barCount.setEnabled(true);
    newBpm.setEnabled(true);
}

void MainComponent::stop()
{
    
    recorder.stopRecording();
    stopTimer();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(false);
    buttonStop.setEnabled(false);
    buttonReset.setEnabled(true);
    buttonDump.setEnabled(true);

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
    buttonReset.setEnabled(true);
    buttonStop.setEnabled(false);
}

void MainComponent::sendBufferVals()
{
    recorder.createBuffer(barCount.getValue(), newBpm.getValue());
}

void MainComponent::timerCallback()
{
    if (!recorder.isRecording)
    {
        done();
    }
}



