/*
  ==============================================================================

    onsetDetection.h
    Created: 14 Oct 2020 2:38:45pm
    Author:  Walter Kopacz

  ==============================================================================
*/

#include "recordAudio.h"
#include "classifyAudio.h"
#include <cmath>
#include <math.h>

#pragma once
class onsetDetection
{

public:

    static constexpr auto fftOrder = 10; // The order of the fft; nfft = 2^order
    static constexpr auto fftSize = 1 << fftOrder; // Size of fft in binary
    static constexpr auto hopLength = 768;

    onsetDetection();
    ~onsetDetection();
    
    void makeNoveltyFunction(juce::AudioBuffer<float>buffer, int audioNumOfSamples);
    void pickPeaks(std::vector<float>noveltyFunction);
    void convertIndiciesToTime(std::vector<int>peaksInIndicies);
    void testSegmentation();
    void convertTimeToSamples(std::vector<float>peaksInTime);
    
    std::vector<std::vector<float>>fftData;
    std::vector<float>noveltyFunction;
    std::vector<int>peaks;
    std::vector<float>peaksInSeconds;
    std::vector<float>peaksInSamples;
    

private:
    
   
    juce::dsp::FFT forwardFFT; // FFT object to perform forward fft on
    juce::dsp::WindowingFunction<float> hannWindow; 
    recordAudio Audio;
    classifyAudio classification;
        
    
};
