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

}

buildMIDI::~buildMIDI()
{
    midiOutput->stopBackgroundThread();
    midiBuffer.clear();
}

void buildMIDI::prepareToPlay(juce::MidiOutput* output, double sampleRate)
{
    midiOutput = output;
    mSampleRate = sampleRate;
}

void buildMIDI::fillBuffer(const int* sampleArray, const int* drumArray, const int* velocityArray)
{
    //creates MIDI messages and sends them into a buffer to be later outputted
    for (auto i = 0; i < sizeof(sampleArray); ++i)
    {
        auto message = noteOn(1, drumArray[i], (juce::uint8)velocityArray[i]);
        midiBuffer.addEvent(message, sampleArray[i]);

        auto messageOff = noteOff(message.getChannel(), message.getNoteNumber());
        midiBuffer.addEvent(messageOff, sampleArray[i] + 1);
    }

    midiOutput->startBackgroundThread();
}


void buildMIDI::outputMIDI()
{
    midiOutput->sendBlockOfMessages(midiBuffer, 0.0, mSampleRate);
}