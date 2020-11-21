/*
  ==============================================================================

    classifyAudio.h
    Created: 11 Nov 2020 8:04:20pm
    Author:  JohnK

  ==============================================================================
*/

#include <JuceHeader.h>
#include <cmath>
#include <math.h>

#pragma once

class classifyAudio
{
 
    static constexpr auto fftOrder = 10; // The order of the fft; nfft = 2^order
    static constexpr auto fftSize = 1 << fftOrder; // Size of fft in binary
    static constexpr auto hopLength = 768;
    static constexpr auto melFilterNum = 128;
    static constexpr auto dctFilterNum = 18;

    
public:
    classifyAudio();
    ~classifyAudio();

    std::vector<double> normalizeFeatures(std::vector<double> featureVec);
    void splitAudio(juce::AudioBuffer<float>buffer, std::vector<int>peaks, double sampleRate);
    std::vector<std::vector<float>> doFFT(std::vector<float> audio);
    double freqToMel(double freq);
    double melToFreq(double mel);
    std::vector<std::vector<float>> getMelFilterBank(double sampleRate);
    std::vector<double> linspace(double start_in, double end_in, int num_in);
    std::vector<double> arange(double start_in, double end_in, double spacing);
    std::vector<std::vector<float>> doFilter(std::vector<std::vector<float>> signal_power, std::vector<std::vector<float>> mel_basis);
    std::vector<std::vector<float>> signalPower(std::vector<std::vector<float>> fftData);
    std::vector<std::vector<float>> constructDCT(std::vector<std::vector<float>> signal_filtered);
    std::vector<std::vector<float>> normalize(std::vector<std::vector<float>> weights, std::vector<double> mel_f);
    std::vector<std::vector<float>> dotProduct(std::vector<std::vector<float>> matrix1, std::vector<std::vector<float>> matrix2);
    std::vector<double> meanMfcc(std::vector<std::vector<float>> matrix);
    void tester(juce::AudioBuffer<float> buffer, double sampleRate);
    void testAccuracy(std::vector<std::vector<float>> cepCoeff);
    void testAccuracy1(std::vector<float> section);

private:

    juce::dsp::FFT forwardFFT; // FFT object to perform forward fft on

    double mSampleRate{ 0 };
    double pi = 3.1415926535897932385;
    std::vector<double> minVals = { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3 };
    std::vector<double> maxVals = { 3, 4, 5, 3, 4, 5, 3, 4, 5, 3, 4, 5, 3, 4, 5, 3, 4, 5 };

    juce::AudioFormatManager mFormatManager;
};
