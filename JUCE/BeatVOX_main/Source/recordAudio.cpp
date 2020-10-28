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
    auto* device = deviceManager.getCurrentAudioDevice();
 /*   deviceSetup = deviceManager.getAudioDeviceSetup();*/
    audioSetupComp.reset(new juce::AudioDeviceSelectorComponent(deviceManager, 1, 1, 2, 2, false, true, false, true));
    audioSetupComp->setSize(600, 400);

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
    bufferRecordedAudio.clear();
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
            
            bufferToFill.buffer->clear(0, remainder);

            startSample += mBufferSize;
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

            bufferToFill.buffer->clear(0, mBufferSize);
            
            startSample += mBufferSize;
        }
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
    deviceSetup = deviceManager.getAudioDeviceSetup();

    auto activeInputChannels = device->getActiveInputChannels();
    numInputChannels = activeInputChannels.getHighestBit() + 1;

    auto activeOutputChannels = device->getActiveOutputChannels();
    numOutputChannels = activeOutputChannels.getHighestBit() + 1;

    if ((device == nullptr) || (numInputChannels == 0) || (numOutputChannels != 2))
    {
        errored = true;
    }
    else
    {
        errored = false;
 
        mSampleRate = sampleRate;
        mBufferSize = samplesPerBlockExpected;
        
       
        midiOutput = deviceManager.getDefaultMidiOutput();
        if (midiOutput != nullptr)
            midiOutput->startBackgroundThread();

        metronome.prepareToPlay(mBufferSize, mSampleRate);

        createBuffer(mBar, mBpm);
    }
    
}

void recordAudio::releaseResources()
{
    mSampleRate = 0.0;
}


void recordAudio::startRecording()
{
    isRecording = true;
}

void recordAudio::stopRecording()
{
    isRecording = false;
    bufferRecordedAudio.setSize(numInputChannels, startSample, true, true, false);
}

void recordAudio::resetRecording()
{
    isRecording = false;
    startSample = 0;
    metronome.reset();

    createBuffer(mBar, mBpm);
    midiBuffer.clear();
}

void recordAudio::createBuffer(int numBar, double bpm)
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
    midiBuffer.clear();

    for (auto i = 0; i < onsetArray.size(); ++i)
    {
        auto message = juce::MidiMessage::noteOn(1, drumArray[i], (juce::uint8) 100);
        midiBuffer.addEvent(message, onsetArray[i]);

        auto messageOff = juce::MidiMessage::noteOff(message.getChannel(), message.getNoteNumber());
        midiBuffer.addEvent(messageOff, onsetArray[i] + 5000);
    }
}


void recordAudio::outputMidi()
{
    DBG(midiOutput->getName());
    midiOutput->sendBlockOfMessages(midiBuffer, juce::Time::getMillisecondCounter(), mSampleRate);
    
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
