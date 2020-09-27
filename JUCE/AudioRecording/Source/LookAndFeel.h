#pragma once

#include <JuceHeader.h>
#include "AudioRecording.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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

            //auto baseColour = state ? juce::Colours::red : backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 0.2f : 0.9f)
              //  .withMultipliedAlpha(button.isEnabled() ? 4.0f : 0.1f);

            auto baseColour = state ? juce::Colours::red : backgroundColour;

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

