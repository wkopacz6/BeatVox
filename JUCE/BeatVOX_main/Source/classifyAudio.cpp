/*
  ==============================================================================

    classifyAudio.cpp
    Created: 11 Nov 2020 8:04:20pm
    Author:  JohnK

  ==============================================================================
*/

#include "classifyAudio.h"


void classifyAudio::splitAudio(juce::AudioBuffer<float>buffer, std::vector<int>peaks, double sampleRate)
{
    mSampleRate = sampleRate;

    getFilterPoints(mSampleRate);
    auto filters = constructFilterBank();
    normFilters = normalize(filters);
    auto dctFilters = constructDCT();

    std::vector<float>audio(buffer.getNumSamples(), 0);
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        audio[i] = buffer.getSample(0, i);
    }

    for (auto i = 0; i < peaks.size(); i++)
    {
        auto length = int(mSampleRate * 0.04);
        int numberOfZeros = fftSize - (length) % (hopLength);
        auto section = std::vector<float>(length + numberOfZeros, 0);

        auto start_ind = peaks[i];
        auto end_ind = peaks[i] + section.size();

        if (end_ind > audio.size())
        {
            end_ind = audio.size();
        }
        
        for (auto j = start_ind; j < end_ind; j++)
        {
            section[j-start_ind] = audio[j];
        }

        auto fft = doFFT(section);
        auto signal_power = signalPower(fft);
        auto audio_filtered = doFilter(signal_power);
        auto cepCoeff = dotProduct(dctFilters, audio_filtered);
    }
}

std::vector<std::vector<float>> classifyAudio::doFFT(std::vector<float> audio)
{
    int numOfFFTs = ceil((audio.size()) / (hopLength));
    std::vector<std::vector<float>> fftData;
    fftData.resize(numOfFFTs + 1);


    for (int i = 0; i < numOfFFTs + 1; i++) {
        std::vector<float> audioData(fftSize * 2);

        // Prepare for FFT
        for (int n = 0; n < (fftSize); n++) {
            float hannWindowMultiplier = 0.5 * (1 - cos(2 * pi * n / 1023));
            audioData[n] = hannWindowMultiplier * audio[n + (hopLength * i)];
        }
        // JUCE FFT
        forwardFFT.performFrequencyOnlyForwardTransform(audioData.data());

        // Convert to dB
        for (int j = 0; j < audioData.size(); j++) {
            audioData[j] = 20 * log10(audioData[j] + 1e-12);
        }
        // Take only positive frequency part of fft
        std::vector<float>posfftData(fftSize / 2, 0);
        fftData[i].resize(fftSize / 2);
        if (i == 0) {
            for (int j = 0; j < fftSize / 2; j++) {
                posfftData[j] = audioData[j];
            }
            fftData[i] = posfftData;
            fftData[i + 1] = posfftData;
            i++;
        }
        else {

            for (int j = 0; j < fftSize / 2; j++) {
                posfftData[j] = audioData[j];
            }
            fftData[i] = posfftData;


        }
    }

    return fftData;
}

std::vector<std::vector<float>> classifyAudio::normalize(std::vector<std::vector<int>> filters)
{
    std::vector<double> enorm;
    auto normFilter = std::vector<std::vector<float>>(filters.size(), std::vector<float>(filters[0].size()));
    for (auto i = 0; i < 128; i++) {
        enorm[i] = 1.0 / (freqs[i + 2] - freqs[i]);

    }
    for (auto i = 0; i < filters.size(); i++) {
        for(auto j = 0; j < filters[i].size(); j++) {
            normFilter[i][j] = (double)filters[i][j] * enorm[i];
        }
        
    }

    return normFilter;
}

std::vector<std::vector<float>> classifyAudio::signalPower(std::vector<std::vector<float>> fftData)
{
    auto power = std::vector<std::vector<float>>(fftData.size(), std::vector<float>(fftData[0].size()));

    for (auto i = 0; i < fftData.size() ; i++) {
        for (auto j = 0; j < fftData[0].size() ; j++) {
            power[i][j] = pow(abs(fftData[i][j]), 2);
        }
    
    }
    return power;
}

double classifyAudio::freqToMel(double freq)
{
    return(2595.0 * log10(1.0 + freq / 700.0));
}

double classifyAudio::melToFreq(double mel)
{
    return 700.0 * ( pow(10, (mel / 2595.0)) - 1.0);
}

void classifyAudio::getFilterPoints(double sampleRate)
{
    auto fmin_mel = freqToMel(0);
    auto fmax_mel = freqToMel(sampleRate/2);
    std::vector<double>mels = linspace(fmin_mel, fmax_mel, melFilterNum + 2);
    freqs = std::vector<double>(mels.size(), 0);

    for (auto i = 0; i < mels.size(); i++)
    {
        freqs[i] = melToFreq(mels[i]);
    }

    filterpoints = std::vector<int>(freqs.size(), 0);
    for (auto i = 0; i < freqs.size(); i++)
    {
        filterpoints[i] = (int)(floor(fftSize + 1) * sampleRate * freqs[i]);
    }
}


std::vector<std::vector<int>> classifyAudio::constructFilterBank() 
{
    auto filters = std::vector<std::vector<int>>(filterpoints.size()-2, std::vector<int>((int)fftSize/2 + 1));

    for (auto n = 0; n < filterpoints.size() - 2; n++)
    {
        auto lin1 = linspace(0, 1, filterpoints[n + 1] - filterpoints[n]);
        auto lin2 = linspace(1, 0, filterpoints[n + 2] - filterpoints[n + 1]);
        for (auto j = filterpoints[n]; j <= filterpoints[n + 1]; j++)
        {
            for (auto i = 0; i < lin1.size(); i++)
                filters[n][j] = lin1[i];
        }

        for (auto j = filterpoints[n + 1]; j <= filterpoints[n + 2]; j++)
        {
            for (auto i = 0; i < lin2.size(); i++)
                filters[n][j] = lin2[i];
        }
    }

    return filters;
}

std::vector<std::vector<float>> classifyAudio::doFilter(std::vector<std::vector<float>> signal_power)
{
    std::vector<std::vector<float> > trans_vec(signal_power[0].size(), std::vector<float>(signal_power.size()));

    for (int i = 0; i < signal_power.size(); i++)
    {
        for (int j = 0; j < signal_power[i].size(); j++)
        {
            trans_vec[j][i] = signal_power[i][j];
        }
    }

    auto dot = dotProduct(normFilters, trans_vec);
    std::vector<std::vector<float> > audio_log(dot.size(), std::vector<float>(dot[0].size()));

    for (auto i = 0; i < audio_log.size(); i++) {
        for (auto j = 0; j < audio_log[0].size(); j++) {
            audio_log[i][j] = 10.0*log10f(dot[i][j]);
        }
    }

    return (audio_log);

}

std::vector<std::vector<float>> classifyAudio::constructDCT()
{
    std::vector<std::vector<float>> basis(dctFilterNum, std::vector<float>(melFilterNum));

    for (auto i = 0; i < basis[0].size(); i++)
    {
        basis[0][i] = 1.0 / sqrt(melFilterNum);
    }

    auto samples = arange(1, 2 * melFilterNum, 2);

    for (auto i = 0; i < samples.size(); i++)
    {
        samples[i] = pi / (2 * melFilterNum);
    }

    for (auto i = 1; i < basis.size(); i++)
        for (auto j = 0; j < basis[0].size(); j++)
            basis[i][j] = cos(i * samples[j]) * sqrt(2.0 / melFilterNum);

    return basis;
}

std::vector<std::vector<float>> classifyAudio::dotProduct(std::vector<std::vector<float>> matrix1, std::vector<std::vector<float>> matrix2)
{
    std::vector<std::vector<float>> output(matrix1.size(), std::vector<float>(matrix2[0].size()));
    double currentSum = 0;
    int index;

    if (matrix1[0].size() != matrix2.size())
    {
        DBG("cannot perform dot product");
    }

    for (int i = 0; i < matrix1.size(); i++) 
    {
        index = 0;
        for (int j = 0; j < matrix1[0].size(); j++)
        {
            currentSum += matrix1[i][j] * matrix2[j][i];
            index += 1;
        }     
        output[i][index] = currentSum;        
    }
    return output;
}

std::vector<double> classifyAudio::arange(double start, double end, double spacing)
{
    std::vector<double>aranged;
    double start = static_cast<double>(start);
    double end = static_cast<double>(end);
    double num = static_cast<double>(spacing);
    double current = start;
    aranged.push_back(start);
 

    while (current < end) 
    {
            aranged.push_back(current + spacing);
            current += spacing;
    }
    return aranged;
}

std::vector<double> classifyAudio::linspace(double start_in, double end_in, int num_in)
{

    std::vector<double> linspaced;

    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for (int i = 0; i < num - 1; ++i)
    {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end); 
    return linspaced;
}

