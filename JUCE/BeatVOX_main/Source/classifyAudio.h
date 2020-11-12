/*
  ==============================================================================

    classifyAudio.h
    Created: 11 Nov 2020 8:04:20pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
#include "onsetDetection.h"
#pragma once

class classifyAudio
{

public:
    void splitAudio(juce::AudioBuffer<float>buffer, std::vector<int>peaks);
    std::vector<std::vector<int>> constructFilterBank();
    double freqToMel(double freq);
    double melToFreq(double mel);
    void getFilterPoints(double fmin, double fmax, int sampleRate);
    std::vector<double> linspace(double start_in, double end_in, int num_in);
    std::vector<std::vector<double>> signalPower();
    std::vector<std::vector<float>> normalize(std::vector<std::vector<int>> filters);
    

private:

    std::vector<int> filterpoints;
    std::vector<double> freqs;
    onsetDetection onset;

};