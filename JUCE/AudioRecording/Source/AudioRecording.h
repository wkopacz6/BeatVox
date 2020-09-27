/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioRecording
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Records audio to a file.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        AudioRecording

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

//#include "DemoUtilities.h"
#include "LookAndFeel.h"

//==============================================================================

class AudioRecorder  : public AudioIODeviceCallback
{
public:
    AudioRecorder (AudioThumbnail& thumbnailToUpdate)
        : thumbnail (thumbnailToUpdate)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override
    {
        stop();
    }

    //==============================================================================
    void startRecording (const File& file)
    {
        stop();

        if (sampleRate > 0)
        {
           
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream()))
            {
                
                WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor (fileStream.get(), sampleRate, 1, 16, {}, 0))
                {
                    fileStream.release(); 

                    threadedWriter.reset (new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768));

                    thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    const ScopedLock sl (writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        {
            const ScopedLock sl (writerLock);
            activeWriter = nullptr;
        }
        threadedWriter.reset();
    }

    bool isRecording() const
    {
        return activeWriter.load() != nullptr;
    }

    //==============================================================================
    void audioDeviceAboutToStart (AudioIODevice* device) override
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void audioDeviceStopped() override
    {
        sampleRate = 0;
    }

    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples) override
    {
        const ScopedLock sl (writerLock);

        if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
        {
            activeWriter.load()->write (inputChannelData, numSamples);

          
            AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
            thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
            nextSampleNum += numSamples;
        }

        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                FloatVectorOperations::clear (outputChannelData[i], numSamples);
    }

private:
    AudioThumbnail& thumbnail;
    TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double sampleRate = 0.0;
    int64 nextSampleNum = 0;

    CriticalSection writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

//==============================================================================
class RecordingThumbnail  : public Component,
                            private ChangeListener
{
public:
    RecordingThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener (this);
    }

    ~RecordingThumbnail() override
    {
        thumbnail.removeChangeListener (this);
    }

    AudioThumbnail& getAudioThumbnail()     { return thumbnail; }

    void setDisplayFullThumbnail (bool displayFull)
    {
        displayFullThumb = displayFull;
        repaint();
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
        g.setColour (Colours::lightgrey);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto endTime = displayFullThumb ? thumbnail.getTotalLength()
                                            : jmax (30.0, thumbnail.getTotalLength());

            auto thumbArea = getLocalBounds();
            thumbnail.drawChannels (g, thumbArea.reduced (2), 0.0, endTime, 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No file recorded)", getLocalBounds(), Justification::centred, 2);
        }
    }

private:
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache  { 10 };
    AudioThumbnail thumbnail            { 512, formatManager, thumbnailCache };

    bool displayFullThumb = false;

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordingThumbnail)
};

//==============================================================================
class AudioRecording  : public Component
{
public:
    AudioRecording()
    {
        setOpaque(true);
        setLookAndFeel(&otherlookandfeel);

        buttonRecord.setClickingTogglesState(true);
        buttonRecord.setToggleState(false, juce::NotificationType::dontSendNotification);
        addAndMakeVisible(buttonRecord);

        buttonPlay.setClickingTogglesState(true);
        buttonPlay.setToggleState(false, juce::NotificationType::dontSendNotification);
        addAndMakeVisible(buttonPlay);

        addAndMakeVisible(recordingThumbnail);
        addAndMakeVisible(buttonAnalyze);
        
        buttonPlay.onClick = [this]
        {
            if (buttonRecord.getToggleState() && buttonPlay.getToggleState())
                startRecording();
            else
                stopRecording();
        };

        audioDeviceManager.initialise(2, 2, nullptr, true, {}, nullptr);
        audioDeviceManager.addAudioCallback (&recorder);

        setSize(300, 200);
    }

    ~AudioRecording() override
    {
        setLookAndFeel(nullptr);
        audioDeviceManager.removeAudioCallback (&recorder);
    }

    void paint (Graphics& g) override
    {
        g.fillAll(juce::Colours::black);

        g.setColour(juce::Colours::whitesmoke);
        g.setFont(juce::Font("Times New Roman", 20.0f, juce::Font::bold));
        g.drawText("BeatVOX", getWidth()/4, 5, getWidth()/2, 30, juce::Justification::centred, true);

        if (buttonRecord.getToggleState()) {
            buttonRecord.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
            buttonRecord.setButtonText("Armed to Record");
            
        }
        else {

            buttonRecord.setButtonText("Press to Arm Recording");
        }

        if (buttonPlay.getToggleState()) {
            buttonPlay.setButtonText("Stop");
            if (buttonRecord.getToggleState()) {
                buttonRecord.setButtonText("Recording...");
            }          
        }
        else {
            buttonPlay.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
            buttonPlay.setButtonText("Play");
        }

    }

    void resized() override
    {
        auto border = 4;

        auto area = getLocalBounds().reduced(5);
        auto buttonHeight = 30;

        area.removeFromTop(buttonHeight).reduced(border);
        buttonRecord.setBounds(area.removeFromTop(buttonHeight).reduced(border));
        buttonPlay.setBounds(area.removeFromTop(buttonHeight).reduced(border));
        recordingThumbnail.setBounds(area.removeFromTop(80).reduced(8));
        buttonAnalyze.setBounds(area.removeFromTop(buttonHeight).reduced(border));   
        
        
    }

private:
    
    AudioDeviceManager audioDeviceManager;
    RecordingThumbnail recordingThumbnail;
    AudioRecorder recorder  { recordingThumbnail.getAudioThumbnail() };
    TextButton recordButton;
    File lastRecording;

    void startRecording()
    {

        auto parentDir = File::getSpecialLocation (File::userDocumentsDirectory);
      

        lastRecording = parentDir.getNonexistentChildFile ("JUCE Demo Audio Recording", ".wav");
        recorder.startRecording (lastRecording);

        recordingThumbnail.setDisplayFullThumbnail (false);
    }

    void stopRecording()
    {
        recorder.stop();

        lastRecording = File();
        recordingThumbnail.setDisplayFullThumbnail (true);
    }


private:
    OtherLookAndFeel otherlookandfeel;
    juce::TextButton buttonRecord;
    juce::TextButton buttonPlay;
    juce::TextButton buttonAnalyze{ "Analyze" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioRecording)
};
