/*
  ==============================================================================

    onsetDetection.cpp
    Created: 14 Oct 2020 2:38:45pm
    Author:  Walter Kopacz

  ==============================================================================
*/
#include <cmath>
#include "onsetDetection.h"

onsetDetection::onsetDetection() : forwardFFT(fftOrder) {

     
}

onsetDetection:: ~onsetDetection() {
    
}

void onsetDetection::makeNoveltyFunction(juce::AudioBuffer<float>buffer, int audioNumOfSamples) {

    //zero pad
    int numberOfZeros = fftSize - (audioNumOfSamples%(hopLength));
    // turn buffer into vector
    std::vector<float>audio(audioNumOfSamples+numberOfZeros, 0);
    for(int i = 0; i < audioNumOfSamples; i++){
        audio[i] = buffer.getSample(0, i);
    }
    // Normalize the audio
    float absmax = 0;
    for (size_t i = 0; i < audioNumOfSamples; ++i)
    {
        absmax = std::max(absmax, std::abs(audio[i]));
    }
    for (size_t i = 0; i < audioNumOfSamples; ++i)
    {
        audio[i] = audio[i] / absmax;
    }
    // Allocate array for FFT data
    
    int numOfFFTs = ceil((audioNumOfSamples)/(hopLength));
    fftData.resize(numOfFFTs + 1);
    
    
    for(int i = 0; i < numOfFFTs + 1; i++) {
        std::vector<float> audioData(fftSize*2);
        
        // Prepare for FFT
        for (int n = 0; n < (fftSize); n++){
            audioData[n] = audio[n+(hopLength*i)];
        }
        
        // JUCE FFT
        forwardFFT.performFrequencyOnlyForwardTransform(audioData.data());
        
        // Convert to dB
        for(int j = 0; j < audioData.size(); j++){
            audioData[j] = 20*log10(audioData[j]+1e-12);
        }
        if (i == 0){
            fftData[i] = audioData;
            fftData[i+1] = (audioData);
            i++;
        }
        else {
        fftData[i] = audioData;
        }


    }
    
    // Diff each spectrum
    std::vector<std::vector<float>> flux(fftData.size()-1);
    for(int i = 0; i < flux.size(); i++){ flux[i].resize(fftSize*2); }
    
    //Only take first half of the fft (non-negative frequencies)
    for(int i = 0; i < fftData.size() - 1; i++){
        for(int j = 0; j < (fftData[i].size()); j++){
            flux[i][j] = pow((fftData[i][j] - fftData[i+1][j]), 2);
            // square for RMS
        }
    }
    
    //Halfwave rectification
    for(int i = 0; i < flux.size(); i++){
        for(int j = 0; j < flux[i].size(); j++){
            if (flux[i][j] < 0) {
                flux[i][j] = 0;
            }
        }
    }
    
    // RMS
    std::vector<float>mean(numOfFFTs);
    
    for(int i = 0; i < flux.size(); i++){
        std::vector<float> sum(flux[i].size());
        for(int j = 0; j < flux[i].size(); j++){
            sum[i] = sum[i] + flux[i][j];
        }
        mean[i] = sum[i] / flux[i].size();
    }
    
    std::vector<float>RMS(mean.size());
    for(int i = 0; i < mean.size(); i++){
        RMS[i] = sqrt(mean[i]);
    }
    // Normalize novelty function
    float absmax_nov = 0;
    for (size_t i = 0; i < RMS.size(); ++i)
    {
        absmax_nov = std::max(absmax_nov, std::abs(RMS[i]));
    }
    for (size_t i = 0; i < RMS.size(); ++i)
    {
        RMS[i] = RMS[i] / absmax_nov;
    }
    noveltyFunction = RMS;
    
}

void onsetDetection::pickPeaks(std::vector<float>noveltyFunction) {
    
    // Get adaptive threshold using median filter ( 5 points )
    std::vector<float>adaptiveThreshold(noveltyFunction.size(), 0);
    std::vector<float>noveltyFunctionZeroPadded(noveltyFunction.size()+4, 0); //for now assuming it adds 2 zero at the beginning and end
    for(int i = 2; i < noveltyFunctionZeroPadded.size(); i++){
        if (i < noveltyFunction.size()+2){
            noveltyFunctionZeroPadded[i] = noveltyFunction[i-2];
        }
    }
    std::array<float, 5>currentBlock;
    for(int i = 2; i < noveltyFunctionZeroPadded.size()-2; i++){
        currentBlock[0] = noveltyFunctionZeroPadded[i-2];
        currentBlock[1] = noveltyFunctionZeroPadded[i-1];
        currentBlock[2] = noveltyFunctionZeroPadded[i];
        currentBlock[3] = noveltyFunctionZeroPadded[i+1];
        currentBlock[4] = noveltyFunctionZeroPadded[i+2];
        std::sort(currentBlock.begin(), currentBlock.end());
        adaptiveThreshold[i-2] = currentBlock[2] + .2;
    }
    //pick the peaks
    for(int i = 1; i < noveltyFunction.size(); i++){
        if (noveltyFunction[i-1] < noveltyFunction[i] &&
            noveltyFunction[i+1] < noveltyFunction[i] &&
            noveltyFunction[i] > adaptiveThreshold[i]){
            peaks.push_back(i);
            i = i+3;
            
        }
    }
}
void onsetDetection::convertIndiciesToTime(std::vector<int>peaksInIndicies){
    for (int i = 0; i < peaksInIndicies.size(); i++){
        peaksInSeconds.push_back(peaksInIndicies[i] * hopLength / Audio.mSampleRate);
    }
}

void onsetDetection::testSegmentation(){
    juce::File myFile;

    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    myFile = parentDir.getChildFile("Test_Segmentation.csv");
    myFile.deleteFile();

    juce::FileOutputStream output2(myFile);
    output2.writeString("Novelty Function\n");
    //Output npvelty fucntion to CSV
    for (auto sample = 0; sample < noveltyFunction.size(); ++sample)
    {
        juce::String dataString1 = (juce::String) noveltyFunction[sample];
        output2.writeString(dataString1);
        if (sample <= peaks.size()){
            juce::String dataString2 = (juce::String) peaks[sample];
            juce::String dataString3 = (juce::String) peaksInSeconds[sample];
            output2.writeString(",");
            output2.writeString(dataString2);
            output2.writeString(",");
            output2.writeString(dataString3);
        }

        output2.writeString("\n");
    }
    output2.flush();
    myFile.~File();
    DBG("Done writing");
}


    

