#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel(&otherlookandfeel);

    
    
    buttonRecord.setClickingTogglesState(true);
    buttonRecord.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(buttonRecord);

    buttonPlay.setClickingTogglesState(true);
    buttonPlay.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(buttonPlay);

    addAndMakeVisible(buttonAnalyze);

    setSize(300, 200);
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{

    g.fillAll(juce::Colours::lightgrey);

    if (buttonPlay.getToggleState()) {
        buttonPlay.setButtonText("Stop");
    }
    else {
        buttonPlay.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        buttonPlay.setButtonText("Play");
    }

    if (buttonRecord.getToggleState()) {
        buttonRecord.setButtonText("Recording...");
        buttonRecord.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    }
    else {

        buttonRecord.setButtonText("Press to Record");
    }
   
}

void MainComponent::resized()
{
    auto border = 4;

    auto area = getLocalBounds();
    auto buttonHeight = 30;

    buttonRecord.setBounds(area.removeFromTop(buttonHeight).reduced(border));
    buttonPlay.setBounds(area.removeFromTop(buttonHeight).reduced(border));
    buttonAnalyze.setBounds(area.removeFromTop(buttonHeight).reduced(border));
}
