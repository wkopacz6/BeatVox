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
  /* midiList = midiOutput->getAvailableDevices();*/
}

buildMIDI::~buildMIDI()
{
    //midiOutput->stopBackgroundThread();
    midiBuffer.clear();
}

void buildMIDI::setDevice(juce::MidiOutput *output)
{

  /*  midiOutput = output->rel;
    DBG(midiOutput->getName());
    midiOutput->startBackgroundThread();*/

}

void buildMIDI::fillBuffer(juce::Array<int> onsetArray, juce::Array<int> drumArray, juce::Array<int> velocityArray)
{
    //creates MIDI messages and sends them into a buffer to be later outputted
    for (auto i = 0; i < onsetArray.size(); ++i)
    {
        auto message = noteOn(1, drumArray[i], (juce::uint8) 100);
        midiBuffer.addEvent(message, onsetArray[i]);

        auto messageOff = noteOff(message.getChannel(), message.getNoteNumber());
        midiBuffer.addEvent(messageOff, onsetArray[i] + 5000);
    }

}

void buildMIDI::outputMIDI(double sampleRate, juce::MidiOutput *midiOutput, juce::MidiBuffer midiBuffer)
{
    DBG(midiOutput->getName());
    midiOutput->sendBlockOfMessages(midiBuffer, juce::Time::getMillisecondCounter(), sampleRate);
    midiBuffer.clear();
}