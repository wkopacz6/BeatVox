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
    setAudioChannels(2, 2);
    auto* device = deviceManager.getCurrentAudioDevice();
    deviceSetup = deviceManager.getAudioDeviceSetup();
    
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

    if (isRecording)
    {
        mBufferSize = bufferToFill.numSamples;

        if ((startSample + mBufferSize) >= bufferRecordedAudio.getNumSamples())
        {
            auto remainder = bufferRecordedAudio.getNumSamples() - startSample;
            for (auto channel = 0; channel < numInputChannels; ++channel)
            {
                auto* readerInput  = bufferToFill.buffer->getReadPointer(channel);
                auto* writerOutput = bufferToFill.buffer->getWritePointer(channel);
                auto* writerRecord = bufferRecordedAudio .getWritePointer(channel, startSample);
                for (auto sample = 0; sample < remainder; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                    writerOutput[sample] = 0;
                }
            }
            startSample += mBufferSize;
            stopRecording();
        }
        else
        {
            for (auto channel = 0; channel < numInputChannels; ++channel)
            {
                auto* readerInput  = bufferToFill.buffer->getReadPointer(channel);
                auto* writerOutput = bufferToFill.buffer->getWritePointer(channel);
                auto* writerRecord = bufferRecordedAudio .getWritePointer(channel, startSample);
                for (auto sample = 0; sample < mBufferSize; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                    writerOutput[sample] = 0;
                }
            }
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

    deviceName = deviceSetup.inputDeviceName;
    mSampleRate = sampleRate;
    mBufferSize = samplesPerBlockExpected;

    auto midiOutput = deviceManager.getDefaultMidiOutput();
    midi.prepareToPlay(midiOutput, mSampleRate);
    metronome.prepareToPlay(mBufferSize, mSampleRate);
    
    createBuffer(mBar, mBpm);
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


void recordAudio::tester()
{
    juce::File myFile;

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    myFile = parentDir.getChildFile("Test_Audio_Recording.csv");
    myFile.deleteFile();

    juce::FileOutputStream output2(myFile);
    output2.writeString("Channel1,Channel2\n");
    
    for (auto sample = 0; sample < bufferRecordedAudio.getNumSamples(); ++sample)
    {
        
        juce::String dataString1 = (juce::String) bufferRecordedAudio.getSample(0, sample);
        output2.writeString(dataString1);
        
        output2.writeString(",");

        juce::String dataString2 = (juce::String) bufferRecordedAudio.getSample(1, sample);
        output2.writeString(dataString2);

        output2.writeString("\n");
    }
    output2.flush();  
    myFile.~File();
    DBG("Done writing");
}
