/*
  ==============================================================================

    buildMIDI.cpp
    Created: 18 Oct 2020 6:04:23pm
    Author:  JohnK

  ==============================================================================
*/

#include "buildMIDI.h"

buildMIDI::buildMIDI()
{
    midiList = midiOutput->getAvailableDevices();
}

buildMIDI::~buildMIDI()
{
    out->stopBackgroundThread();
    midiBuffer.clear();
}

void buildMIDI::setDevice(int selected)
{
    auto output = midiList[selected];
    out = midiOutput->openDevice(output.identifier);
    out->startBackgroundThread();
}

void buildMIDI::setSampleRate(double sampleRate)
{
    mSampleRate = sampleRate;
}

void buildMIDI::fillBuffer(juce::Array<int> onsetArray, juce::Array<int> drumArray, juce::Array<int> velocityArray)
{
    //creates MIDI messages and sends them into a buffer to be later outputted
    for (auto i = 0; i < onsetArray.size(); ++i)
    {
        auto message = noteOn(1, drumArray[i], (juce::uint8)velocityArray[i]);
        midiBuffer.addEvent(message, onsetArray[i]);
       
        auto messageOff = noteOff(message.getChannel(), message.getNoteNumber());
        midiBuffer.addEvent(messageOff, onsetArray[i] + 10);
    }

}


void buildMIDI::outputMIDI()
{
    out->sendBlockOfMessages(midiBuffer, juce::Time::getMillisecondCounter(), mSampleRate);
    midiBuffer.clear();
}