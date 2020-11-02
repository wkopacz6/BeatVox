#include "MainComponent.h"


//==============================================================================
MainComponent::MainComponent()
{
    setOpaque(true);

    //initalizies openable window that allows user to configure audio devices and midi output
    menuAudioSetup.reset(new juce::DialogWindow("Settings", juce::Colours::black, true, true));

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

    addAndMakeVisible(audioSetupButton);
    audioSetupButton.setEnabled(true);
    audioSetupButton.onClick = [this]() { ioSetup(); };

    addAndMakeVisible(errorBox);
    errorBox.setReadOnly(true);
    errorBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    errorBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    errorBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    addAndMakeVisible(buttonAnalyze);
    buttonAnalyze.setEnabled(false);
    buttonAnalyze.onClick = [this]() { buttonAnalyzePressed(); };

    addAndMakeVisible(buttonPlayMidi);
    buttonPlayMidi.setEnabled(false);
    buttonPlayMidi.onClick = [this]() { buttonPlayMidiPressed(); };

    //Checks if audio devices are properly configured on startup
    if (recorder.errored)
    {
        error();
        errorBox.setText("ERROR -- RECONFIGURE AUDIO DEVICES");
    }
    else
    {
        reset();
        errorBox.setText("Audio Devices Successfully configured");
        sendBufferVals();
    }

    //When user changes audio devices, the changeListenerCallback function is called
    recorder.deviceManager.addChangeListener(this);

    setSize(450, 430);


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
    buttonRecord    .setBounds(10,  50, getWidth() - 20, 30);
    buttonStop      .setBounds(10,  90, getWidth() - 20, 30);
    buttonReset     .setBounds(10, 130, getWidth() - 20, 30);
    buttonDump      .setBounds(10, 170, getWidth() - 20, 30);
    barCount        .setBounds(10, 210, getWidth() - 20, 30);
    newBpm          .setBounds(10, 250, getWidth() - 20, 30);
    buttonMet       .setBounds(10, 290, getWidth() /  4, 30);
    errorBox        .setBounds(10, 405, getWidth() - 20, 20);
    buttonAnalyze   .setBounds(getWidth()/4, 330, getWidth() /  2, 30);
    buttonPlayMidi  .setBounds((getWidth()/4) + 10, 370, (getWidth()/2) - 20, 20);
    audioSetupButton.setBounds(360, 15, getWidth() /  8, 20);
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
    segments.makeNoveltyFunction(recorder.bufferRecordedAudio, recorder.bufferRecordedAudio.getNumSamples());
    segments.pickPeaks(segments.noveltyFunction);
    segments.convertIndiciesToTime(segments.peaks);
    segments.testSegmentation();
}

//graphically informs the user that recording has finished
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
    recorder.createAudioBuffer(barCount.getValue(), newBpm.getValue());
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

void MainComponent::buttonAnalyzePressed()
{
    juce::Array<int> onsetArray =    {0, 22050, 44100, 66150, 88200, 132300, 154350};
    juce::Array<int> drumArray =     {36, 48, 38, 48, 36, 38, 38 };
    juce::Array<int> velocityArray = {100,100,100,100,100,100,100 };

    
    recorder.fillMidiBuffer(onsetArray, drumArray, velocityArray);
}

void MainComponent::buttonPlayMidiPressed()
{
    recorder.outputMidi();
}

void MainComponent::ioSetup()
{
    menuAudioSetup->showDialog("Settings", recorder.audioSetupComp.get(), this, juce::Colours::black, true, false, false);
}

//If there is an audio device error, the user cannot perform any functions until they reconfigure their devices in settings
void MainComponent::error()
{
    buttonRecord  .setEnabled(false);
    buttonMet     .setEnabled(false);
    barCount      .setEnabled(false);
    newBpm        .setEnabled(false);
    buttonAnalyze .setEnabled(false);
    buttonPlayMidi.setEnabled(false); 
}

//Periodically checks if recordAudio class is still in a recording state
void MainComponent::timerCallback()
{
    if (!recorder.isRecording)
    {
        done();
    }
}

//Handles when user changes audio devices anytime after startup
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (recorder.errored)
    {
        error();
        errorBox.setText("ERROR -- RECONFIGURE AUDIO DEVICES");
    }
    else
    {
        if (recorder.deviceManager.getDefaultMidiOutput() != nullptr)
        {
            buttonAnalyze .setEnabled(true);
            buttonPlayMidi.setEnabled(true);
            buttonPlayMidi.setButtonText("Play");
        }
        else
        {
            buttonAnalyze .setEnabled(false);
            buttonPlayMidi.setEnabled(false);
            buttonPlayMidi.setButtonText("<Set Midi Output>");
        }

        reset();
        errorBox.setText("Audio Devices Successfully configured");
    }

}
     
    


