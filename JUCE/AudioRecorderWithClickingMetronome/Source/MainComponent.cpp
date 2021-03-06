#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setOpaque(true);

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

    addAndMakeVisible(buttonMet);
    buttonMet.setEnabled(true);
    buttonMet.setClickingTogglesState(true);
    buttonMet.setToggleState(false, true);
    buttonMet.onClick = [this]() { metPressed(); };

    addAndMakeVisible(inputBox);
    inputBox.setReadOnly(true);
    inputBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    inputBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    inputBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    addAndMakeVisible(outputBox);
    outputBox.setReadOnly(true);
    outputBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    outputBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    outputBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));
    
    if (recorder.errored)
    {
        error();
        inputBox.setText("ERROR -- PLEASE RECONFIGURE I/O DEVICES AND RESTART APP");
        outputBox.setText("--------------------------------------");
    }
    else
    {
        sendBufferVals();
        inputBox.setText("Input Device: " + recorder.deviceSetup.inputDeviceName);
        outputBox.setText("Output Device: " + recorder.deviceSetup.outputDeviceName);
    }

    recorder.deviceManager.addChangeListener(this);
    setSize (450, 380);

}



MainComponent::~MainComponent()
{
    recorder.deviceManager.removeChangeListener(this);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::antiquewhite);
    g.setFont(juce::Font("Times New Roman", 20.0f, juce::Font::bold));
    g.drawText("BeatVOX", getWidth()/4, 10, getWidth()/2, 30, juce::Justification::centred, true);
}

void MainComponent::resized()
{
    buttonRecord.setBounds(10, 50, getWidth() - 20, 30);
    buttonStop  .setBounds(10, 90, getWidth() - 20, 30);
    buttonReset .setBounds(10, 130, getWidth() - 20, 30);
    buttonDump  .setBounds(10, 170, getWidth() - 20, 30);
    barCount    .setBounds(10, 210, getWidth() - 20, 30);
    newBpm      .setBounds(10, 250, getWidth() - 20, 30);
    buttonMet   .setBounds(10, 290, getWidth() / 4, 30);
    inputBox    .setBounds(10, 330, getWidth() - 20, 20);
    outputBox   .setBounds(10, 355, getWidth() - 20, 20);

}



void MainComponent::start()
{
   
    recorder.startRecording();
    startTimer(50);

    buttonRecord.setButtonText("Recording...");
    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    buttonRecord.setEnabled(false);
    buttonReset .setEnabled(false);
    buttonStop  .setEnabled(true);
    barCount    .setEnabled(false);
    newBpm      .setEnabled(false);
    buttonMet   .setEnabled(false);

    
}

void MainComponent::reset()
{
    recorder.resetRecording();
    stopTimer();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(true);
    buttonReset .setEnabled(false);
    buttonDump  .setEnabled(false);
    barCount    .setEnabled(true);
    newBpm      .setEnabled(true);
    buttonMet   .setEnabled(true);
}

void MainComponent::stop()
{
    
    recorder.stopRecording();
    stopTimer();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(false);
    buttonStop  .setEnabled(false);
    buttonReset .setEnabled(true);
    buttonDump  .setEnabled(true);

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

    buttonDump .setEnabled(true);
    buttonReset.setEnabled(true);
    buttonStop .setEnabled(false);
}

void MainComponent::sendBufferVals()
{
    recorder.createBuffer(barCount.getValue(), newBpm.getValue());
}

void MainComponent::metPressed()
{
    if (metEnable)
    {
        metEnable = false;
        buttonMet.setButtonText("Metronome Off");
    }
    else
    {
        metEnable = true;
        buttonMet.setButtonText("Metronome On");
    }
    recorder.metEnabled(metEnable);
}

void MainComponent::error()
{
    buttonRecord.setEnabled(false);
    buttonMet   .setEnabled(false);
    barCount    .setEnabled(false);
    newBpm      .setEnabled(false);
}

void MainComponent::timerCallback()
{
    if (!recorder.isRecording)
    {
        done();
    }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    inputBox.setText("ERROR -- PLEASE RESTART APPLICATION");
    outputBox.setText("--------------------------------------");
    error();
}


