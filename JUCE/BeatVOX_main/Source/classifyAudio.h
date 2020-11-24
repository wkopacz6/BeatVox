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
    std::vector<double> minVals48000 = { -647.96295166, -7.15366364, -155.70796204, -37.43126297, -57.08383179, -47.80818176, -63.70907593, -50.35643387, -53.85018539, -43.19771194, -31.30822754, -21.14154625, -26.07251549, -37.53669357, -30.06274223, -19.2313633, -30.5000782, -21.15400314 };
    std::vector<double> maxVals48000 = { -36.69165039, 252.31211853, 73.0195694, 133.45358276, 73.54243469, 77.36062622, 74.84476471, 48.87318039, 42.0711174, 37.73397827, 33.96343613, 34.62980652, 24.7007885, 29.49488831, 28.95998383, 29.46679115,  24.88707542,  30.30918312 };
    std::vector<double> minVals44100 = {-638.05584717, -29.19802284, -151.52670288, -39.24344254, -61.95689011, -35.33233261, -61.00283432, -64.59571838, -41.2538681, -51.83400345, -31.27592278, -25.45388222, -28.49080849, -37.02992249, -32.1645546, -24.17269897, -21.82624054, -19.50192833};
    std::vector<double> maxVals44100 = { -23.18715858, 237.5743866, 68.46765137, 125.84372711, 74.8282547, 76.80852509, 70.79082489, 47.69644165, 39.9026947, 32.89352798, 33.56898117, 39.49528122, 21.03834724, 28.64596558, 33.27782822, 31.86940575, 20.48654938, 32.4284668 };
    juce::AudioFormatManager mFormatManager;
};
