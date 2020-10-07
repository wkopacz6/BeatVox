/*
  ==============================================================================

    recordAudio.cpp
    Created: 2 Oct 2020 12:47:40pm
    Author:  JohnK

  ==============================================================================
*/

#include "recordAudio.h"


recordAudio::recordAudio()
{
    createBuffer(96000);
    
}

recordAudio::~recordAudio()
{
    bufferRecordedAudio.clear();
}

void recordAudio::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
    float** outputChannelData, int numOutputChannels, int numSamples)
{
    
    if (isRecording)
    {
        if ((startSample+numSamples) >= bufferRecordedAudio.getNumSamples())
        {
            auto remainder = bufferRecordedAudio.getNumSamples() - startSample;
            for (auto channel = 0; channel < numInputChannels ; ++channel)
            {
                auto* writer = bufferRecordedAudio.getWritePointer(channel, startSample);
                for (auto sample = 0; sample < remainder; ++sample)
                {
                    writer[sample] = inputChannelData[channel][sample]; 
                }
            }
            stopRecording();
        }
        else 
        {
            for (auto channel = 0; channel < numInputChannels ; ++channel)
            {
                auto* writer = bufferRecordedAudio.getWritePointer(channel, startSample);
                for (auto sample = 0; sample < numSamples; ++sample)
                {
                    writer[sample] = inputChannelData[channel][sample];
                }
            }
            startSample += numSamples;  
        }
    }
}

void recordAudio::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}

void recordAudio::audioDeviceStopped()
{
    sampleRate = 0.0;
}


void recordAudio::startRecording()
{
    isRecording = true;
}

void recordAudio::stopRecording()
{
    isRecording = false;
    DBG("Done Recording");
   
}

void recordAudio::resetRecording()
{
    isRecording = false;
    startSample = 0;
    bufferRecordedAudio.clear();
    
}

void recordAudio::createBuffer(int samplesAllocated)
{
    bufferRecordedAudio.setSize(2, samplesAllocated);
    bufferRecordedAudio.clear();
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
