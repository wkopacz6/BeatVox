#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : state(Stopped), buttonFile("Open..."), playButton("Play"), stopButton("Stop")

{
    setSize(800, 600);
    
    //'Open file' button
    buttonFile.onClick = [this] { buttonFileClicked(); };
    addAndMakeVisible(&buttonFile);

    //'Play' button
    playButton.onClick = [this] {playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(true);
    addAndMakeVisible(&playButton);

    //'Stop' button
    stopButton.onClick = [this] {stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);
    addAndMakeVisible(&stopButton);

    //initializing transport and audio file format manager
    formatManager.registerBasicFormats();
    transport.addChangeListener(this);

    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
   
    shutdownAudio();
    //shuts down transport when application is exitted
    transport.setSource(nullptr);
}

void MainComponent::playButtonClicked() {
    //When 'Play' button is clicked, state is changed to 'starting'
    transportStateChanged(Starting);
}

void MainComponent::stopButtonClicked() {
    //When 'Stop' button is clicked, state is changed to 'stopping'
    transportStateChanged(Stopping);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &transport) {
        if (transport.isPlaying()) {
            transportStateChanged(Playing);
        }
        else {
            transportStateChanged(Stopped);
        }
    }
}

void MainComponent::transportStateChanged(transportState newState) {
    //Compares new state to current state
    if (newState != state) {
        state = newState;

        //Switch statement that properly initializes features for when the state changes
        switch (state)
        {
        case MainComponent::Stopped:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transport.setPosition(0.0);
            break;
        case MainComponent::Starting:
            stopButton.setEnabled(true);
            playButton.setEnabled(false);
            transport.start();
            break;
        case MainComponent::Stopping:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transport.stop();
            break;
        case MainComponent::Playing:
            break;
        default:
            break;
        }
    }
}

void MainComponent::buttonFileClicked() {
    
    //Opens a dialog box and allows user to choose a file
    juce::FileChooser chooser("Choose a WAV file to import", juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        { "*.wav" }, true, false, nullptr);

    //If the user chooses a file, create a reader for such file
    if (chooser.browseForFileToOpen()) {
        juce::File fileImport;
        fileImport = chooser.getResult();
        juce::AudioFormatReader* reader = formatManager.createReaderFor(fileImport);


        if (reader != nullptr) {
            //Creates a unique pointer to hold the reader
            std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(reader, true));

            //Set the source of the transport to this user-selected file and reset playback
            transport.setSource(tempSource.get());
            transportStateChanged(Stopped);
            playSource.reset(tempSource.release());
        }
        
        
        
    }

}
//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();

    transport.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
   
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

   
}

void MainComponent::resized()
{
    
    buttonFile.setBounds(10, 10, getWidth()-20, 30);
    playButton.setBounds(10, 50, getWidth() - 20, 30);
    stopButton.setBounds(10, 90, getWidth() - 20, 30);
}
