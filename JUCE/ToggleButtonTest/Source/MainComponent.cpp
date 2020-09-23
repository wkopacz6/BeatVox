#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(600, 400);
    addAndMakeVisible(button);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    
    button.setButtonText("Record");

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::red);
    g.fillRect(getWidth() / 4, getHeight() / 4, getWidth() / 2, getHeight() / 2);

    g.setColour(juce::Colours::white);
    g.setFont(26.0f);
    g.drawText("BeatVOX", getWidth() / 4, 20, getWidth() / 2, 40,
        juce::Justification::centred, true);

}

void MainComponent::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    juce::Rectangle<int> areaButton = area.removeFromBottom(area.getHeight()/2);
    button.setBounds(areaButton.removeFromRight(area.getWidth()/2));
}
