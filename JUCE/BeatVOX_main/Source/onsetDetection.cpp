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

void onsetDetection::makeNoveltyFunction(float audio[], int audioNumOfSamples) {
    
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
    int fftSize = forwardFFT.getSize();
    int numOfFFTs = ceil(audioNumOfSamples/(hopLength));
    std::vector<std::vector<float>> fftData;
       
    for(int i = 0; i < numOfFFTs; i++) {
        std::vector<float> audioData(fftSize*2);
        
        // Prepare for FFT
        for (int n = 0; n < (fftSize*2)-1; n++){
            audioData[n] = audio[n+(hopLength*i)];
        }
        
        // JUCE FFT
        forwardFFT.performFrequencyOnlyForwardTransform(audioData.data());
        
        // Convert to dB
        for(int j = 0; j < audioData.size(); j++){
            audioData[j] = 20*log10(audioData[j]+1e-12);
        }
        
        fftData[i] = audioData;
    }
    
    // Diff each spectrum
    std::vector<std::vector<float>> flux(fftData.size() - 1);
    for(int i = 0; i < flux.size(); i++){ flux[i].resize(fftSize*2); }
    
    for(int i = 0; i < fftData.size() - 1; i++){
        for(int j = 0; j < fftData[i].size(); j++){
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
    std::vector<float>mean(flux[0].size(), 0);
    std::vector<float> sum(flux[0].size(), 0);
    for(int i = 0; i < flux.size(); i++){
        for(int j = 0; j < flux[i].size(); j++){
            sum[i] = sum[i] + flux[i][j];
        }
        mean[i] = sum[i] / flux[i].size();
    }
    
    std::vector<float>RMS(mean.size(), 0);
    for(int i = 0; i < sum.size(); i++){
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
        adaptiveThreshold[i-2] = currentBlock[2];
    }
    //pick the peaks
    std::vector<int>peaks;
    for(int i = 1; i < noveltyFunction.size(); i++){
        if (noveltyFunction[i-1] < noveltyFunction[i] &&
            noveltyFunction[i+1] < noveltyFunction[i] &&
            noveltyFunction[i] > adaptiveThreshold[i]){
            peaks.push_back(noveltyFunction[i]);
            i = i+3;
            
        }
    }
}
void onsetDetection::convertIndiciesToTime(std::vector<int>peaksInIndicies){
    std::vector<float>peaksInSeconds(peaksInIndicies.size(), 0);
    for (int i = 0; i < peaksInIndicies.size(); i++){
        peaksInSeconds[i] = peaksInIndicies[i] * hopLength / Audio.mSampleRate;
    }
}

