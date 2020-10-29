/*
  ==============================================================================

    Metronome.cpp
    Created: 9 Oct 2020 9:37:58am
    Author:  JohnK

  ==============================================================================
*/

#include "Metronome.h"
Metronome::Metronome()
{
    mFormatManager.registerBasicFormats();

    juce::File myFile{ juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDesktopDirectory) };
    auto mySamples = myFile.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "Cowbell-2.wav");

    jassert(mySamples[0].exists());

    auto formatReader = mFormatManager.createReaderFor(mySamples[0]);

    pMet.reset(new juce::AudioFormatReaderSource(formatReader, true));
}
void Metronome::setBpm(int bpm)
{
    mBpm = bpm;
    mSampleInterval = (60.0 / mBpm) * mSampleRate;
}

void Metronome::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mSampleRate = sampleRate;
    mSampleInterval = (60.0 / mBpm) * mSampleRate;

    if (pMet != nullptr)
    {
        pMet->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void Metronome::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
  
    if (onMet)
    {
        if (mTotalSamples == 0)
        {
            pMet->setNextReadPosition(0);
            pMet->getNextAudioBlock(bufferToFill);
        }

        auto bufferSize = bufferToFill.numSamples;

        auto mSamplesToGo = mTotalSamples % mSampleInterval;

        //prepares for incoming click
        if (((mSamplesToGo + bufferSize) >= mSampleInterval))
        {
            //finds exact number of samples remaining until click should occur
            auto timeToStartClick = mSampleInterval - mSamplesToGo - 1;

            
            pMet->setNextReadPosition(0);
            //counts samples once it detects impending click and determines sample for click
            for (auto sample = 0; sample < bufferSize; sample++)
            {
                //chooses sample to play click at
                if (sample == timeToStartClick)
                {
                    pMet->getNextAudioBlock(bufferToFill);
                }
            }

        }
        mTotalSamples += bufferSize;

    }
 
}

void Metronome::reset()
{
    mTotalSamples = 0;
}

