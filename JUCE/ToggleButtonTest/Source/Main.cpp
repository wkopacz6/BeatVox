/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class ToggleButtonTestApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    ToggleButtonTestApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {

        mainWindow.reset (new MainWindow ("ToggleButtonTest", new MainComponent, *this));
    }

    void shutdown() override
    {

        mainWindow = nullptr; 
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
       
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
      
    }

   
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name, juce::Component* c, JUCEApplication& a)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons),
            app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits(300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }


    private:
        JUCEApplication& app;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (ToggleButtonTestApplication)
