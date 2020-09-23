#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel(&otherlookandfeel);

    
    buttonRecord.setButtonText("Press to Record");
    buttonRecord.setClickingTogglesState(true);
    buttonRecord.setToggleState(false, juce::NotificationType::dontSendNotification);
    addAndMakeVisible(buttonRecord);

    buttonPlay.setButtonText("Play");
    addAndMakeVisible(buttonPlay);

    buttonStop.setButtonText("Stop");
    addAndMakeVisible(buttonStop);
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
    if (buttonRecord.getToggleState()) {
        buttonRecord.setButtonText("Recording...");
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
    buttonStop.setBounds(area.removeFromTop(buttonHeight).reduced(border));
}
