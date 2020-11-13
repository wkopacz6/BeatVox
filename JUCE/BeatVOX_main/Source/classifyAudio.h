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

    static constexpr auto fftOrder = 10; // The order of the fft; nfft = 2^order
    static constexpr auto fftSize = 1 << fftOrder; // Size of fft in binary
    static constexpr auto hopLength = 768;

public:
    void splitAudio(juce::AudioBuffer<float>buffer, std::vector<int>peaks, double sampleRate);
    std::vector<std::vector<float>> doFFT(std::vector<float> audio);
    std::vector<std::vector<int>> constructFilterBank();
    double freqToMel(double freq);
    double melToFreq(double mel);
    void getFilterPoints(double sampleRate);
    std::vector<double> linspace(double start_in, double end_in, int num_in);
    std::vector<std::vector<float>> signalPower(std::vector<std::vector<float>> fftData);
    std::vector<std::vector<float>> normalize(std::vector<std::vector<int>> filters);
    
private:

    std::vector<int> filterpoints;
    std::vector<double> freqs;
    onsetDetection onset;

    juce::dsp::FFT forwardFFT; // FFT object to perform forward fft on
    juce::dsp::WindowingFunction<float> hannWindow;

    double mSampleRate{ 0 };

};