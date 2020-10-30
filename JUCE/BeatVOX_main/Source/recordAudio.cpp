/*
  ==============================================================================

    recordAudio.cpp
    Created: 2 Oct 2020 12:47:40pm
    Author:  JohnK

  ==============================================================================
*/
#include <iostream>
#include "recordAudio.h"


recordAudio::recordAudio()
{
    setAudioChannels(1, 2);

    //inherited JUCE class that handles changing/initializing audio devices
    audioSetupComp.reset(new juce::AudioDeviceSelectorComponent(deviceManager, 1, 1, 2, 2, false, true, false, true));
    audioSetupComp->setSize(600, 400);

    //Checks if audio devices are configured correctly on startup
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device == nullptr)
    {
        errored = true;
    }
    else
    {
        errored = false;
    }
}

recordAudio::~recordAudio()
{
    bufferRecordedAudio.setSize(0,0);
    bufferMidi.clear();

    shutdownAudio();
}



void recordAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    if ((isRecording) && (numInputChannels != 0))
    {

        mBufferSize = bufferToFill.numSamples;

        if ((startSample + mBufferSize) >= bufferRecordedAudio.getNumSamples())
        {
            auto remainder = bufferRecordedAudio.getNumSamples() - startSample;
            
            auto* readerInput  = bufferToFill.buffer->getReadPointer(0);
            auto* writerRecord = bufferRecordedAudio .getWritePointer(0, startSample);

                for (auto sample = 0; sample < remainder; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                }
            
            startSample += remainder;
            stopRecording();
        }
        else
        {
            auto* readerInput = bufferToFill.buffer->getReadPointer(0);
            auto* writerRecord = bufferRecordedAudio.getWritePointer(0, startSample);
                
                for (auto sample = 0; sample < mBufferSize; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                }
            
            startSample += mBufferSize;
        }
        bufferToFill.buffer->clear(0, mBufferSize);
        metronome.getNextAudioBlock(bufferToFill);
        
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void recordAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    auto* device = deviceManager.getCurrentAudioDevice();
 
    auto activeInputChannels = device->getActiveInputChannels();
    numInputChannels = activeInputChannels.getHighestBit() + 1;

    auto activeOutputChannels = device->getActiveOutputChannels();
    numOutputChannels = activeOutputChannels.getHighestBit() + 1;

    //Ensures selected audio device has 1 input and 2 outputs
    if ((device == nullptr) || (numInputChannels == 0) || (numOutputChannels != 2))
    {
        errored = true;
    }
    else
    {
        errored = false;
 
        mSampleRate = sampleRate;
        mBufferSize = samplesPerBlockExpected;

        metronome.prepareToPlay(mBufferSize, mSampleRate);

        createAudioBuffer(mBar, mBpm);
    }

    midiOutput = deviceManager.getDefaultMidiOutput();
    if (midiOutput != nullptr)
        midiOutput->startBackgroundThread();
    
}

void recordAudio::releaseResources()
{

}


void recordAudio::startRecording()
{
    isRecording = true;
}

void recordAudio::stopRecording()
{
    isRecording = false;

    //Changes length of buffer after recording has stopped.  Only makes a difference if recording was stopped prematurely
    bufferRecordedAudio.setSize(numInputChannels, startSample, true, true, false);
}

void recordAudio::resetRecording()
{
    isRecording = false;
    startSample = 0;
    metronome.reset();

    createAudioBuffer(mBar, mBpm);
}

void recordAudio::createAudioBuffer(int numBar, double bpm)
{
    mBar = numBar;
    mBpm = bpm;
    metronome.setBpm(mBpm);

    auto samplesToAllocate = mBar * (60.0 / mBpm) * mSampleRate * 4;
    bufferRecordedAudio.setSize(numInputChannels, samplesToAllocate);
    bufferRecordedAudio.clear();
}

void recordAudio::metEnabled(bool enable)
{
    metronome.onMet = enable;
}

void recordAudio::fillMidiBuffer(juce::Array<int> onsetArray, juce::Array<int> drumArray, juce::Array<int> velocityArray)
{
    bufferMidi.clear();
    bufferMidi.ensureSize(onsetArray.size());
    for (auto i = 0; i < onsetArray.size(); ++i)
    {
        auto message = juce::MidiMessage::noteOn(1, drumArray[i], (juce::uint8) 100);
        bufferMidi.addEvent(message, onsetArray[i]);

        auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), message.getNoteNumber());
        bufferMidi.addEvent(messageOff, onsetArray[i] + 5000);
    }

}


void recordAudio::outputMidi()
{
    midiOutput->sendBlockOfMessages(bufferMidi, juce::Time::getMillisecondCounter(), mSampleRate);
}


void recordAudio::tester()
{
    juce::File myFile;

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    myFile = parentDir.getChildFile("Test_Audio_Recording.csv");
    myFile.deleteFile();

    juce::FileOutputStream output2(myFile);
    output2.writeString("Channel1\n");
    
    for (auto sample = 0; sample < bufferRecordedAudio.getNumSamples(); ++sample)
    {
        
        juce::String dataString1 = (juce::String) bufferRecordedAudio.getSample(0, sample);
        output2.writeString(dataString1);

        output2.writeString("\n");
    }
    output2.flush();  
    myFile.~File();
    DBG("Done writing");
}

