#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel(&otherlookandfeel);

    
    button.setButtonText("Record");
    addAndMakeVisible(button);
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

   /* g.fillAll(juce::Colours::black);

    
    button.setButtonText("Record");

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::red);
    g.fillRect(getWidth() / 4, getHeight() / 4, getWidth() / 2, getHeight() / 2);

    g.setColour(juce::Colours::white);
    g.setFont(26.0f);
    g.drawText("BeatVOX", getWidth() / 4, 20, getWidth() / 2, 40,
        juce::Justification::centred, true);
        */
}

void MainComponent::resized()
{
    auto border = 4;

    auto area = getLocalBounds();
    auto buttonHeight = 30;

    button.setBounds(area.removeFromTop(buttonHeight).reduced(border));
}
