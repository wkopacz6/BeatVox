/*
  ==============================================================================

    classifyAudio.cpp
    Created: 11 Nov 2020 8:04:20pm
    Author:  JohnK

  ==============================================================================
*/

#include "classifyAudio.h"

classifyAudio::classifyAudio() : forwardFFT(fftOrder){};

classifyAudio::~classifyAudio() {};

void classifyAudio::tester(juce::AudioBuffer<float> buffer, double sampleRate)
{


    std::vector<int> peaks = { 0 };
    //juce::AudioBuffer<float> buffer(1, 40000);
    //buffer.setSize(1, 40000, true);
    //buffer.clear();

    splitAudio(buffer, peaks, sampleRate);

}

void classifyAudio::splitAudio(juce::AudioBuffer<float>buffer, std::vector<int>peaks, double sampleRate)
{
    mSampleRate = sampleRate;

    mFormatManager.registerBasicFormats();

    juce::File myFile{ juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory) };
    auto mySamples = myFile.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "snare.wav");

    auto reader = mFormatManager.createReaderFor(mySamples[0]);
    juce::AudioSampleBuffer bufferTest(reader->numChannels, reader->lengthInSamples);
    reader->read(&bufferTest, 0, reader->lengthInSamples, 0, true, true);

    std::vector<float>audio(bufferTest.getNumSamples(), 0);
    for (int i = 0; i < bufferTest.getNumSamples(); i++) {
       audio[i] = bufferTest.getSample(0, i);
     }

    /*getFilterPoints(mSampleRate);
    auto filters = constructFilterBank();
    normFilters = normalize(filters);*/
    auto mel_basis = getMelFilterBank(mSampleRate);
    auto dctFilters = constructDCT();

    /*std::vector<float>audio(buffer.getNumSamples(), 0);
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        audio[i] = buffer.getSample(0, i);
    }*/

    for (auto i = 0; i < peaks.size(); i++)
    {
        auto length = int(mSampleRate * 0.04);
        auto remainder = (length - fftSize + hopLength) % hopLength;

        int numberOfZeros = 0;
        if (remainder != 0)
            numberOfZeros = hopLength - remainder;

        auto section = std::vector<float>(length + numberOfZeros, 0);

        auto start_ind = peaks[i];
        auto end_ind = peaks[i] + length;

        if (end_ind > audio.size())
        {
            end_ind = audio.size();
        }

        if (i != (peaks.size() - 1))
        {
            if (end_ind >= peaks[i+1])
                end_ind = peaks[i+1];
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
    int numOfFFTs = (audio.size() - fftSize + hopLength) / hopLength;
    std::vector<std::vector<float>> fftData(numOfFFTs, std::vector<float>(fftSize / 2 + 1));

    for (int i = 0; i < numOfFFTs; i++) {
    std::vector<float> audioData(fftSize * 2);

        // Prepare for FFT
        for (int n = 0; n < (fftSize); n++) {
            float hannWindowMultiplier = 0.5 * (1 - cos(2 * pi * n / (fftSize - 1)));
            audioData[n] = hannWindowMultiplier * audio[n + (hopLength * i)];
        }

        // JUCE FFT
        forwardFFT.performFrequencyOnlyForwardTransform(audioData.data());

        // Take only positive frequency part of fft
        std::vector<float>posfftData(1 + (fftSize / 2), 0);
        
        for (int j = 0; j <= fftSize / 2; j++) 
        {
             posfftData[j] = audioData[j];
        }
       
        fftData[i] = posfftData;
    }

    return fftData;
}

std::vector<std::vector<float>> classifyAudio::normalize(std::vector<std::vector<float>> filters)
{
    std::vector<float> enorm(melFilterNum, 0);
    auto normFilter = std::vector<std::vector<float>>(filters.size(), std::vector<float>(filters[0].size()));
    for (auto i = 0; i < melFilterNum; i++) {
        enorm[i] = 2.0 / (freqs[i + 2] - freqs[i]);

    }
    for (auto i = 0; i < filters.size(); i++) {
        for(auto j = 0; j < filters[i].size(); j++) {
            normFilter[i][j] = (float)filters[i][j] * enorm[i];
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

std::vector<std::vector<float>> classifyAudio::getMelFilterBank(double sampleRate)
{
    auto fmin_mel = freqToMel(0);
    auto fmax_mel = freqToMel(sampleRate/2);

    std::vector<std::vector<float>> weights(melFilterNum, std::vector<float>(1 + fftSize / 2));
    weights.clear();

    auto fftFreqs = linspace(0, sampleRate / 2, int(1 + fftSize / 2));
    auto mel_f = linspace(fmin_mel, fmax_mel, melFilterNum + 2);

    for (auto i = 0; i < mel_f.size(); i++)
        mel_f[i] = melToFreq(mel_f[i]);

    std::vector<float> fdiff(mel_f.size() - 1, 0);

    for (auto i = 0; i < fdiff.size(); i++)
        fdiff[i] = mel_f[i + 1] - mel_f[i];

    std::vector<std::vector<float>> ramps(mel_f.size(), std::vector<float>(fftFreqs.size()));
    ramps.clear();
    for (auto i = 0; i < mel_f.size(); i++)
        for (auto j = 0; j < fftFreqs.size(); j++)
            ramps[i][j] = mel_f[i] - fftFreqs[j];

    for (auto i = 0; i < melFilterNum; i++)
    {
        std::vector<float>lower(ramps[0].size(), 0);
        std::vector<float>upper(ramps[0].size(), 0);
        for (auto j = 0; j < ramps[0].size(); j++)
        {
            lower[j] = -ramps[i][j] / fdiff[i];
            upper[j] = ramps[i + 2][j] / fdiff[i + 1];
        }

        std::vector<float> minimum(lower.size(), 0);
        for (auto x = 0; x < lower.size(); x++)
        {
            if (lower[x] <= upper[x])
                minimum[x] = lower[x];
            else
                minimum[x] = upper[x];
        }
        
        std::vector<float> maximum(minimum.size(), 0);
        for (auto x = 0; x < minimum.size(); x++)
        {
            if (minimum[x] > maximum[x])
                maximum[x] = minimum[x];
        }

        weights[i] = maximum;

        weights = normalize(weights);

        return weights;

    }

    std::vector<double>mels = linspace(fmin_mel, fmax_mel, melFilterNum + 2);
    freqs = std::vector<double>(mels.size(), 0); 

    for (auto i = 0; i < mels.size(); i++)
    {
        freqs[i] = melToFreq(mels[i]);
    }

    filterpoints = std::vector<int>(freqs.size(), 0);
    for (auto i = 0; i < freqs.size(); i++)
    {
        filterpoints[i] = (int)(floor((fftSize + 1) / sampleRate * freqs[i]));
    }
}


//std::vector<std::vector<float>> classifyAudio::constructFilterBank() 
//{
//    auto filters = std::vector<std::vector<float>>(filterpoints.size()-2, std::vector<float>((int)fftSize/2 + 1));
//
//    for (auto n = 0; n < filterpoints.size() - 2; n++)
//    {
//        auto lin1 = linspace(0, 1, filterpoints[n + 1] - filterpoints[n]);
//        auto lin2 = linspace(1, 0, filterpoints[n + 2] - filterpoints[n + 1]);
//        for (auto j = filterpoints[n]; j < filterpoints[n + 1]; j++)
//        {
//                filters[n][j] = lin1[j - filterpoints[n]];
//        }
//
//        for (auto j = filterpoints[n + 1]; j < filterpoints[n + 2]; j++)
//        {
//                filters[n][j] = lin2[j - filterpoints[n+1]];
//        }
//    }
//
//    return filters;
//}

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
            audio_log[i][j] = 20.0*log10(dot[i][j]);
        }
    }

    return (audio_log);

}

std::vector<std::vector<float>> classifyAudio::constructDCT()
{
    std::vector<std::vector<float>> basis(dctFilterNum, std::vector<float>(melFilterNum));

    for (auto i = 0; i < basis.size(); i++)
    {
        for (auto j = 0; j < basis[0].size(); j++)
        {
            if (j == 0)
                basis[i][j] = sqrt(1.0 / (4 * melFilterNum)) * 2 * cos((2 * i + 1) * (j * pi) / (2 * melFilterNum));
            else
                basis[i][j] = sqrt(1.0 / (2 * melFilterNum)) * 2 * cos((2 * i + 1) * (j * pi) / (2 * melFilterNum));
        }
    }

    return basis;
}

std::vector<std::vector<float>> classifyAudio::dotProduct(std::vector<std::vector<float>> matrix1, std::vector<std::vector<float>> matrix2)
{
     std::vector<std::vector<float>> output(matrix1.size(), std::vector<float>(matrix2[0].size()));

    if (matrix1[0].size() != matrix2.size())
    {
        DBG("cannot perform dot product");
    }
    else
    {
        for (auto i = 0; i < matrix1.size(); i++)
        {
            for (auto j = 0; j < matrix2[0].size(); j++)
            {
                for (auto k = 0; k < matrix1[0].size(); k++)
                {
                    output[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }
    }
    return output;
}

std::vector<double> classifyAudio::arange(double start_in, double end_in, double spacing)
{
    std::vector<double>aranged;
    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(spacing);
    double current = start;
    aranged.push_back(start);
 

    while ((current + spacing) < end) 
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

