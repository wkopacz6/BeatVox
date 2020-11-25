/*
  ==============================================================================

    onsetDetection.cpp
    Created: 14 Oct 2020 2:38:45pm
    Author:  Walter Kopacz

  ==============================================================================
*/

#include <math.h>
#include "onsetDetection.h"

onsetDetection::onsetDetection() : forwardFFT(fftOrder),
hannWindow(fftSize, juce::dsp::WindowingFunction<float>::hann){

     
}

onsetDetection:: ~onsetDetection() {
    peaks.clear();
    peaksInSeconds.clear();
    
}

void onsetDetection::makeNoveltyFunction(juce::AudioBuffer<float>buffer, int audioNumOfSamples) {
    
    
    double pi = 3.141592653589793;
    
    //zero pad
    //int numberOfZeros = fftSize - (audioNumOfSamples%(hopLength));
    // turn buffer into vector
    //std::vector<float>audio(audioNumOfSamples+numberOfZeros, 0);
    std::vector<float>audio(audioNumOfSamples, 0);
    for(int i = 0; i < audioNumOfSamples; i++){
        audio[i] = buffer.getSample(0, i);
    }
    // Normalize the audio
    float absmax = 0;
    for (size_t i = 0; i < audioNumOfSamples; ++i)
    {
        absmax = std::max(absmax, std::abs(audio[i]));
    }
    float x = round(absmax*1000000);
    absmax = x/1000000;
    for (size_t i = 0; i < audioNumOfSamples; ++i)
    {
        audio[i] = audio[i] / absmax;
    }
    // Allocate array for FFT data
    
    //classification.testAccuracy1(audio);

    int numOfFFTs = 1 + int((audio.size() - fftSize) / hopLength);
    std::vector<std::vector<float>> fftData(numOfFFTs, std::vector<float>(fftSize / 2 + 1));
    /*int numOfFFTs = ceil((audioNumOfSamples)/(hopLength));
    fftData.resize(numOfFFTs);*/
    
    for(int i = 0; i < numOfFFTs; i++) {
        std::vector<float> audioData(fftSize*2);
        
        // Prepare for FFT
        for (int n = 0; n < (fftSize); n++){
            double hannWindowMultiplier =(double)(0.5 * (1.0 - cos(2.0 * pi * n / ((double)fftSize))));
            audioData[n] = hannWindowMultiplier * audio[n+(hopLength*i)];
        }
        // JUCE FFT
        forwardFFT.performFrequencyOnlyForwardTransform(audioData.data());
        
        // Convert to dB
       /* for(int j = 0; j < audioData.size(); j++){
            audioData[j] = 20*log10(abs(audioData[j])+1e-12);
        }*/

        // Take only positive frequency part of fft
        std::vector<float>posfftData(1 + (fftSize / 2), 0);

        for (int j = 0; j <= fftSize / 2; j++)
        {
            posfftData[j] = audioData[j];
        }

        fftData[i] = posfftData;
    }
    
    //classification.testAccuracy(fftData);

    //Duplicate the first frame so first flux will be zero
    fftData.insert(fftData.begin(), fftData[0]);
    
    // Diff each spectrum
    std::vector<std::vector<float>> flux(fftData.size()-1);
    for(int i = 0; i < flux.size(); i++){ flux[i].resize(fftSize/2 + 1); }
    
    for(int i = 0; i < fftData.size() - 1; i++){
        for(int j = 0; j < (fftData[i].size()); j++){
            flux[i][j] = fftData[i][j] - fftData[i+1][j];
            
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
    // square for RMS
    for (int i = 0;i < flux.size(); i++) {
        for (int j = 0; j < flux[i].size(); j++) {
            flux[i][j] = pow(flux[i][j], 2);
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
    RMS = mean;
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
    for(int i = 1; i < noveltyFunction.size()-1; i++){
        if (noveltyFunction[i-1] < noveltyFunction[i] &&
            noveltyFunction[i+1] < noveltyFunction[i] &&
            noveltyFunction[i] > adaptiveThreshold[i]){
            peaks.push_back(i);
            i = i+3;
            
        }
    }
}
void onsetDetection::convertIndiciesToTime(std::vector<int>peaksInIndicies){
    if (peaksInIndicies.size() > 0){
        for (int i = 0; i < peaksInIndicies.size(); i++){
            peaksInSeconds.push_back(peaksInIndicies[i] * hopLength / Audio.mSampleRate);
        }
    }
}
void onsetDetection::convertTimeToSamples(std::vector<float>peaksInTime){
    if (peaksInTime.size() > 0){
        for (int i = 0; i < peaksInTime.size(); i++){
            peaksInSamples.push_back(peaksInTime[i] * Audio.mSampleRate);
        }
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
        if (sample < peaks.size()){
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


    

