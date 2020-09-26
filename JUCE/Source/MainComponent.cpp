#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    //set look and feel using the definied otherlookandfeel class
    setLookAndFeel(&otherlookandfeel);

    // add and make visible buttons and initialize state, lambda function for button click
    //buttonRecord.setButtonText("Press to Record");
   // buttonRecord.setClickingTogglesState(true);
   // buttonRecord.setToggleState(false, juce::NotificationType::dontSendNotification);
    //buttonRecord.onClick = [this]() { recordTrigger(); };
   // addAndMakeVisible(buttonRecord);

    buttonPlay.setButtonText("Play");
    addAndMakeVisible(buttonPlay);

    buttonStop.setButtonText("Stop");
    addAndMakeVisible(buttonStop);
    
    // add and make visible Volume Slider and set range, and add lambda function for value change
    addAndMakeVisible(playbackVolume);
    playbackVolume.setRange(0.0, 1.0, 0.01);
    playbackVolume.setValue(1.0);
    // update variable when slider is changed
    playbackVolume.onValueChange=[this] {
        volumevar = playbackVolume.getValue();
    };
    
    // do the same for the Volume Slider Label
    addAndMakeVisible (volumeLabel);
    volumeLabel.setText ("Playback Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent (&playbackVolume, true);
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//stop/record functions to change the playState
void MainComponent::Stop()
{
    //playState=recordState::Stop;
    //buttonRecord.setToggleState(false, juce::NotificationType::dontSendNotification);
    //std::cout << "Stopped" << std::endl;
}
void MainComponent::Record()
{
    //playState=recordState::Record;
   // buttonRecord.setToggleState(true, juce::NotificationType::dontSendNotification);
    //std::cout << "Recording" << std::endl;
}

//triggers stop/record functions
void MainComponent::recordTrigger()
{
   //if (playState == recordState::Record)
    //   Stop();
   //else
    //   Record();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    //grab audio device and channel information
//    auto* device = deviceManager.getCurrentAudioDevice();
//    auto activeInputChannels  = device->getActiveInputChannels();
//    auto activeOutputChannels = device->getActiveOutputChannels();
//    auto maxInputChannels  = activeInputChannels .getHighestBit() + 1;
//    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
//    //access volume slider value and clears the buffer if there are inactive input channels or if there are too many output channels
//    auto level = (float) playbackVolume.getValue();
//
//            for (auto channel = 0; channel < maxOutputChannels; ++channel)
//            {
//                if ((! activeOutputChannels[channel]) || maxInputChannels == 0)
//                {
//                    bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
//                }
//                else
//                {
//                    //retrieves actual channel number
//                    auto actualInputChannel = channel % maxInputChannels; // [1]
//
//                    if (! activeInputChannels[channel]) // [2]
//                    {
//                        bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
//                    }
//                    else // [3]
//                    {
//                        auto* inBuffer = bufferToFill.buffer->getReadPointer (actualInputChannel, bufferToFill.startSample);
//                        auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
//
//                        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
//                            outBuffer[sample] = inBuffer[sample] * level;
//                    }
//                }
//            }

    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
}
void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //set color of component
    g.fillAll(juce::Colours::lightgrey);
        //if (buttonRecord.getToggleState()) {
        //    buttonRecord.setButtonText("Recording...");
       // }
       // else {
       //     buttonRecord.setButtonText("Press to Record");
       // }

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    //add case for button resizing
    auto border = 4;

    auto area = getLocalBounds();
    auto buttonHeight = 30;

    //buttonRecord.setBounds(area.removeFromTop(buttonHeight).reduced(border));
    buttonPlay.setBounds(area.removeFromTop(buttonHeight).reduced(border));
    buttonStop.setBounds(area.removeFromTop(buttonHeight).reduced(border));
    
    //add case for slider resizing
    auto sliderLeft = 120;
    playbackVolume.setBounds (sliderLeft, 85, getWidth() - sliderLeft - 10, 85);
    
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}


