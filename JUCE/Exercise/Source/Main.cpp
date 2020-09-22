/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Button.h"


//==============================================================================
class ExerciseApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    ExerciseApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow("Exercise"));
    }
    void shutdown() override
    {
        mainWindow = nullptr;
    }
    
private:
    class MainWindow : juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name) : DocumentWindow("Exercise", juce::Colours::lightblue, DocumentWindow::allButtons)
        {
            setContentOwned(new Button(), true);
            setUsingNativeTitleBar(true);
            centreWithSize(getWidth(), getHeight());
            setResizable(true, false);
            setVisible(true);
        }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }
private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (ExerciseApplication)
