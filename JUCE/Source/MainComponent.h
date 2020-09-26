#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
// LOOK AND FEEL CLASS FOR BUTTON UI DESIGN
class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel(){}

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {

        auto flatOnLeft = button.isConnectedOnLeft();
        auto flatOnRight = button.isConnectedOnRight();
        auto flatOnTop = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        auto width = (float)button.getWidth() - 1.0f;
        auto height = (float)button.getHeight() - 1.0f;

        if (width > 0 && height > 0)
        {
            auto cornerSize = juce::jmin(15.0f, juce::jmin(width, height) * 0.45f);
            auto lineThickness = cornerSize * 0.1f;
            auto halfThickness = lineThickness * 0.5f;

            juce::Path outline;
            outline.addRoundedRectangle(0.5f + halfThickness, 0.5f + halfThickness, width - lineThickness, height - lineThickness,
                cornerSize, cornerSize,
                !(flatOnLeft || flatOnTop),
                !(flatOnRight || flatOnTop),
                !(flatOnLeft || flatOnBottom),
                !(flatOnRight || flatOnBottom));

            bool state = button.getToggleState();

            auto outlineColour = button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                : juce::TextButton::textColourOffId);

            auto baseColour = state ? juce::Colours::red : backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 0.2f : 0.9f)
                .withMultipliedAlpha(button.isEnabled() ? 4.0f : 0.1f);

            if (isButtonDown || isMouseOverButton)
                baseColour = baseColour.contrasting(isButtonDown ? 0.9f : 0.1f);

            g.setColour(baseColour);
            g.fillPath(outline);

            if (!button.getToggleState())
            {
                g.setColour(outlineColour);
                g.strokePath(outline, juce::PathStrokeType(lineThickness));
            }
        }
    }

    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool isButtonDown) override
    {
        auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
            : juce::TextButton::textColourOffId)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
        auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

        auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
        auto leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        auto rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        //! [drawButtonText]
        auto textWidth = button.getWidth() - leftIndent - rightIndent;

        auto edge = 4;
        auto offset = isButtonDown ? edge / 2 : 0;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
                juce::Justification::centred, 2);
    }
    //! [drawButtonText]
};

//MAIN COMPONENT CLASS
class MainComponent  : public juce::AudioAppComponent
{
public:
    float volumevar;
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void recordTrigger();
    void Stop();
    void Record();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
   
private:
    //==============================================================================
    OtherLookAndFeel otherlookandfeel;
    // sliders
    juce::Slider playbackVolume;
    juce::Label volumeLabel;
    // buttons
    //juce::TextButton buttonRecord;
    juce::TextButton buttonPlay;
    juce::TextButton buttonStop;
    // create playstate class for recording button
    //enum class recordState
    //{
    //    Record,
    //    Stop
    //};
    //recordState playState { recordState::Stop };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

#pragma once

//#include "DemoUtilities.h"

//==============================================================================

class AudioRecorder  : public juce::AudioIODeviceCallback
{
public:
    AudioRecorder (juce::AudioThumbnail& thumbnailToUpdate)
        : thumbnail (thumbnailToUpdate)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override
    {
        stop();
    }

    //==============================================================================
    void startRecording (const juce::File& file)
    {
        stop();

        if (sampleRate > 0)
        {
           
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<juce::FileOutputStream> (file.createOutputStream()))
            {
                
                juce::WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor (fileStream.get(), sampleRate, 1, 16, {}, 0))
                {
                    fileStream.release();

                    threadedWriter.reset (new juce::AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768));

                    thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    const juce::ScopedLock sl (writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        {
            const juce::ScopedLock sl (writerLock);
            activeWriter = nullptr;
        }
        threadedWriter.reset();
    }

    bool isRecording() const
    {
        return activeWriter.load() != nullptr;
    }

    //==============================================================================
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override
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
        const juce::ScopedLock sl (writerLock);

        if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
        {
            activeWriter.load()->write (inputChannelData, numSamples);

          
            juce::AudioBuffer<float>(const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
            thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
            nextSampleNum += numSamples;
        }

        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
    }

private:
    juce::AudioThumbnail& thumbnail;
    juce::TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

//==============================================================================
class RecordingThumbnail  : public Component
                            private juce::ChangeListener
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

    juce::AudioThumbnail& getAudioThumbnail()     { return thumbnail; }

    void setDisplayFullThumbnail (bool displayFull)
    {
        displayFullThumb = displayFull;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::lightgrey);

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
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache  { 10 };
    juce::AudioThumbnail thumbnail            { 512, formatManager, thumbnailCache };

    bool displayFullThumb = false;

    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordingThumbnail)
};

//==============================================================================
class AudioRecordingDemo  : public Component
{
public:
    AudioRecordingDemo()
    {
        setOpaque (true);
        
        addAndMakeVisible (recordButton);
        recordButton.setColour (juce::TextButton::buttonColourId, Colour (0xffff5c5c));
        recordButton.setColour (juce::TextButton::textColourOnId, juce::Colours::black);

        recordButton.onClick = [this]
        {
            if (recorder.isRecording())
                stopRecording();
            else
                startRecording();
        };

        addAndMakeVisible (recordingThumbnail);

        /*RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [this] (bool granted)
                                     {
                                         int numInputChannels = granted ? 2 : 0;
                                         audioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
                                     });*/


        audioDeviceManager.initialise(2, 2, nullptr, true, {}, nullptr);
        audioDeviceManager.addAudioCallback (&recorder);

        setSize (500, 500);
    }

    ~AudioRecordingDemo() override
    {
        audioDeviceManager.removeAudioCallback (&recorder);
    }

    void paint (juce::Graphics& g) override
    {
       
        g.fillAll(juce::Colours::lightgrey);
    }

    void resized() override
    {
        auto area = getLocalBounds();

        recordingThumbnail.setBounds (area.removeFromTop (80).reduced (8));
        recordButton      .setBounds (area.removeFromTop (36).removeFromLeft (140).reduced (8));

    }

private:
    
    juce::AudioDeviceManager audioDeviceManager;
    RecordingThumbnail recordingThumbnail;
    AudioRecorder recorder  { recordingThumbnail.getAudioThumbnail() };
    juce::TextButton recordButton { "Record" };
    juce::File lastRecording;

    void startRecording()
    {
        /*if (! RuntimePermissions::isGranted (RuntimePermissions::writeExternalStorage))
        {
            SafePointer<AudioRecordingDemo> safeThis (this);
            RuntimePermissions::request (RuntimePermissions::writeExternalStorage,
                                         [safeThis] (bool granted) mutable
                                         {
                                             if (granted)
                                                 safeThis->startRecording();
                                         });
            return;
        }*/

        auto parentDir = juce::File::getSpecialLocation (File::userDocumentsDirectory);
      

        lastRecording = parentDir.getNonexistentChildFile ("JUCE Demo Audio Recording", ".wav");

        recorder.startRecording (lastRecording);

        recordButton.setButtonText ("Stop");
        recordingThumbnail.setDisplayFullThumbnail (false);
    }

    void stopRecording()
    {
        recorder.stop();

        lastRecording = File();
        recordButton.setButtonText ("Record");
        recordingThumbnail.setDisplayFullThumbnail (true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioRecordingDemo)
};
