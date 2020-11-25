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
    std::vector<double> minVals44100 = {-562.12286377, -36.23243713, -147.67012024, -23.61817169, -57.86819839, -36.26380539, -37.30766678, -58.18373489, -40.2208519, -38.7626915, -31.27592278, -24.66895866, -27.94661522, -37.02992249, -32.1645546, -21.47883034, -21.60482788, -19.50192833};
    std::vector<double> maxVals44100 = {-23.18715858, 237.33093262, 42.27118301, 132.29042053, 55.314991, 70.72768402, 70.79082489, 46.15379333, 41.93119049, 37.43262863,  33.56898117, 33.31370544, 19.26614952, 28.64596558, 28.21576691, 26.14150047, 21.46536827, 18.79258919};
    std::vector<double> maxVals48000 = {-587.38970947, -19.06607819, -156.47711182, -26.94819069, -56.11130905, -55.74367142, -53.92170715, -49.2865448, -53.85018539, -34.81391525, -31.39482307, -18.50088692, -26.25063133, -37.53669357, -27.44062805, -19.2313633, -30.5000782, -20.69902992};
    std::vector<double> minVals48000 = {-36.69165039, 252.31211853, 44.37088013, 127.7833786, 55.59083939, 79.21143341, 69.17401123, 47.58134079, 40.43525314, 37.69054794, 29.9128933, 34.62980652, 17.57997704, 26.76351738, 24.76544762, 24.9554882, 25.91612244, 23.99448586};
    juce::AudioFormatManager mFormatManager;
};
