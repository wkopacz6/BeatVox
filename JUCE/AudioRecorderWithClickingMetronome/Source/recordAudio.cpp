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
    auto activeInputChannels = device->getActiveInputChannels();
    numInputChannels = activeInputChannels.getHighestBit() + 1;
    
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
        auto buffersize = bufferToFill.numSamples;

        if ((startSample + mBufferSize) >= bufferRecordedAudio.getNumSamples())
        {
            auto remainder = bufferRecordedAudio.getNumSamples() - startSample;
            for (auto channel = 0; channel < numInputChannels; ++channel)
            {
                auto* readerInput = bufferToFill.buffer->getReadPointer(channel);
                auto* writerOutput = bufferToFill.buffer->getWritePointer(channel);
                auto* writerRecord = bufferRecordedAudio.getWritePointer(channel, startSample);
                for (auto sample = 0; sample < remainder; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                    writerOutput[sample] = 0;
                }
            }
            stopRecording();
        }
        else
        {
            for (auto channel = 0; channel < numInputChannels; ++channel)
            {
                auto* readerInput = bufferToFill.buffer->getReadPointer(channel);
                auto* writerOutput = bufferToFill.buffer->getWritePointer(channel);
                auto* writerRecord = bufferRecordedAudio.getWritePointer(channel, startSample);
                for (auto sample = 0; sample < buffersize; ++sample)
                {
                    writerRecord[sample] = readerInput[sample];
                    writerOutput[sample] = 0;
                }
            }
            startSample += buffersize;
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
    mSampleRate = sampleRate;
    mBufferSize = samplesPerBlockExpected;
    metronome.prepareToPlay(mBufferSize, mSampleRate);
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
}

void recordAudio::resetRecording()
{
    isRecording = false;
    startSample = 0;
    metronome.reset();
    bufferRecordedAudio.clear();
}

void recordAudio::createBuffer(int numBar, int bpm)
{
    metronome.getBpm(bpm);
    auto samplesToAllocate = numBar * (60.0 / bpm) * mSampleRate * 4;
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
