/*
  ==============================================================================

    Metronome.cpp
    Created: 9 Oct 2020 9:37:58am
    Author:  JohnK

  ==============================================================================
*/

#include "Metronome.h"

void Metronome::countSamples(int bufferLength)
{
    mTotalSamples += bufferLength;

    auto mSamplesToGo = mTotalSamples % mSampleInterval;

    DBG("Samples Increasing: " << mSamplesToGo);
    DBG("Beat Interval: " << mSampleInterval);

     //prepares for incoming click
    if ((mSamplesToGo + bufferLength) >= mSampleInterval)
    {
        //finds exact number of samples remaining until click should occur
        const auto timeToStartClick = mSampleInterval - mSamplesToGo;
        //resets sound to start of click .wav file
        //pClickSample->setNextReadPosition(0);
        //counts samples once it detects impending click and determines sample for click
        for (auto sample = 1; sample <= bufferLength; sample++)
        {
            //chooses sample to play click at
            if (sample == timeToStartClick)
            {

                DBG("CLICK " << (beat % 4) + 1);
                beat += 1;
                DBG("Total Samples: " << mTotalSamples);

            }
        }

    }
}

void Metronome::reset()
{
    mTotalSamples = 0;
    beat = 1;
}

void Metronome::setInterval(int bpm, double sampleRate)
{
    mSampleInterval = 60.0 / bpm * sampleRate;
}