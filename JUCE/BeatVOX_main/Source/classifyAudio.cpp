/*
  ==============================================================================

    classifyAudio.cpp
    Created: 11 Nov 2020 8:04:20pm
    Author:  JohnK

  ==============================================================================
*/

#include "classifyAudio.h"
#include <cmath>

void classifyAudio::splitAudio(std::vector<float> audio, std::vector<int>peaks)
{
    
    for (auto i = 0; i < peaks.size(); i++)
    {
        auto section = std::vector<float>(5000, 0);
        auto start_ind = peaks[i];
        auto end_ind = peaks[i] + 5000;

        if (end_ind > audio.size())
        {
            end_ind = audio.size();
            section.resize(start_ind + end_ind);
        }
        
        for (auto j = start_ind; j <= end_ind; j++)
        {
            section[j] = audio[j];
        }
    }
}

std::vector<std::vector<float>> classifyAudio::normalize(std::vector<std::vector<int>> filters)
{
    std::vector<double> enorm;
    auto normFilter = std::vector<std::vector<float>>(filters.size(), std::vector<float>(filters[0].size()));
    for (auto i = 0; i < 128; i++) {
        enorm[i] = 2.0 / (freqs[i + 2] - freqs[i]);

    }
    for (auto i = 0; i < filters.size(); i++) {
        for(auto j = 0; j < filters[i].size(); j++) {
            normFilter[i][j] = (double)filters[i][j] * enorm[i];
        }
        
    }

    return normFilter;
}

std::vector<std::vector<double>> classifyAudio::signalPower()
{
    auto power = std::vector<std::vector<double>>(onset.fftData.size(), std::vector<double>(onset.fftData[0].size()));

    for (auto i = 0; i < onset.fftData.size() ; i++) {
        for (auto j = 0; j < onset.fftData[0].size() ; j++) {
            power[i][j] = pow(abs(onset.fftData[i][j]), 2);
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

void classifyAudio::getFilterPoints(double fmin, double fmax, int sampleRate)
{
    auto fmin_mel = freqToMel(fmin);
    auto fmax_mel = freqToMel(fmax);
    std::vector<double>mels = linspace(fmin_mel, fmax_mel, 130);
    freqs = std::vector<double>(mels.size(), 0);

    for (auto i = 0; i < mels.size(); i++)
    {
        freqs[i] = melToFreq(mels[i]);
    }

    filterpoints = std::vector<int>(freqs.size(), 0);
    for (auto i = 0; i < freqs.size(); i++)
    {
        filterpoints[i] = (int)(floor(onset.fftSize + 1) * sampleRate * freqs[i]);
    }
}

std::vector<std::vector<int>> classifyAudio::constructFilterBank() 
{
    auto filters = std::vector<std::vector<int>>(filterpoints.size()-2, std::vector<int>((int)onset.fftSize/2 + 1));

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