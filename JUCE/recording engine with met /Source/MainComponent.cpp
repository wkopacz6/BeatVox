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
    newBpm.setValue(60);
    barCount.setValue(1);
    
    
    //initialize bar slider label and bpm slider label
    addAndMakeVisible (numberOfBarsLabel);
    numberOfBarsLabel.setText ("Number of Bars", juce::dontSendNotification);
    numberOfBarsLabel.attachToComponent (&barCount, true);
    
    addAndMakeVisible (newBpmLabel);
    newBpmLabel.setText ("BPM", juce::dontSendNotification);
    newBpmLabel.attachToComponent (&newBpm, true);
    
    
    
    
    //initialize all buttons and sliders with lambdas
    //set value change detected from sliders
    
    barCount.onValueChange = [this]() { changeBars(); };
    newBpm.onValueChange = [this]() { changeBpm(); };
    
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
    
    setSize (600, 600);
    
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        
        setAudioChannels (2, 2);
    }
}



MainComponent::~MainComponent()
{
    deviceManager.removeAudioCallback(&recorder);
    shutdownAudio();

}

//function for playing metronome
//void MainComponent::playMet()
//{
//    metState = MetState::MetPlaying;
//}

//void MainComponent::stopMet()
//{
 //   metState = MetState::MetStopped;
  //  metronome.reset();
//}

//------------------------------------------------------------=====================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();
    
    //detects if metronome is on, and executes function to count samples
    //if (metState == MetState::MetPlaying)
   // {
    //    metronome.getNextAudioBlock(bufferToFill);
   // }
}

void MainComponent::releaseResources()
{
        
}
    

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    //juce::Rectangle<int> bounds = getLocalBounds().reduced(5);

        //juce::FlexBox flexBox;
        //flexBox.items.add(juce::FlexItem(bounds.getWidth()/4, bounds.getHeight()/4, buttonRecord));
        //flexBox.items.add(juce::FlexItem(bounds.getWidth() / 4, bounds.getHeight() / 4, buttonReset));
        //flexBox.items.add(juce::FlexItem(bounds.getWidth() / 4, bounds.getHeight() / 4, buttonDump));
        //flexBox.items.add(juce::FlexItem(bounds.getWidth() / 4, bounds.getHeight() / 4, buttonStop));
       // flexBox.performLayout(bounds);
    buttonRecord.setBounds(10, 10, getWidth()-20, 30);
    buttonStop.setBounds(10, 50, getWidth() - 20, 30);
    buttonReset.setBounds(10, 90, getWidth() - 20, 30);
    buttonDump.setBounds(10, 130, getWidth() - 20, 30);
    barCount.setBounds(10, 170, getWidth() - 20, 30);
    newBpm.setBounds(10, 210, getWidth() - 20, 30);
}


void MainComponent::changeBars()
{
    recorder.barChanger(barCount.getValue());
}

void MainComponent::changeBpm()
{
    recorder.bpmChanger(newBpm.getValue());
}

void MainComponent::start()
{
    //playMet();
    recorder.startRecording();
    startTimer(500);

    buttonRecord.setButtonText("Recording...");
    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    buttonRecord.setEnabled(false);
    buttonReset.setEnabled(false);
    buttonStop.setEnabled(true);
    barCount.setEnabled(false);
    
}

void MainComponent::reset()
{
    recorder.resetRecording();
    stopTimer();
    //metronome.reset();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(true);
    buttonReset.setEnabled(true);
    buttonDump.setEnabled(false);
    barCount.setEnabled(true);
}

void MainComponent::stop()
{
    recorder.stopRecording();
    //stopMet();
    stopTimer();

    buttonRecord.setButtonText("Record");
    buttonRecord.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));

    buttonRecord.setEnabled(false);
    buttonReset.setEnabled(true);
    buttonDump.setEnabled(true);
    barCount.setEnabled(true);
}

void MainComponent::dumpDataToCSV()
{
    recorder.tester();
}

void MainComponent::done()
{
    stopTimer();
    //stopMet();
    buttonRecord.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    buttonRecord.setButtonText("Done!");

    DBG("Done Recording");
    buttonDump.setEnabled(true);
    buttonReset.setEnabled(true);
    buttonStop.setEnabled(false);
}

void MainComponent::timerCallback()
{
    if (!recorder.isRecording)
    {
        done();
    }
}



