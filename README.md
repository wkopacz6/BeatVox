# BeatVOX

## What is BeatVOX?
BeatVOX is an application that can convert an audio recording of beatboxing into a corresponding MIDI drum pattern, which can be streamed directly into your DAW or to any other MIDI application of your choosing via a virtual MIDI port.
  
## Download and Launching
To download BeatVOX, simply download the ZIP file of all files on this GitHub branch (the "Released" branch).  Then, extract and place these files into your computer's Documents directory.  Keep all files in their original folder layout and do not rename anything.  

### Windows
To launch the application, run the executable inside the "BeatVOX_v1.0" folder.  If files are missing or misplaced, the application will provide an error message.
  
### Mac
Unfortunately, an application file is not available at this time.  To launch the application, the code must be ran in an IDE.  To do this, first ensure JUCE is installed and configured -- the free version is sufficient.  Here is a link: https://juce.com/get-juce.  Also ensure some programming IDE is installed.  Apple's Xcode is a free and JUCE-supported option.  Once this is done, open the "BeatVOX_v1.0.jucer" with Projucer.  Then, click the "export to IDE" button near the top right of the window.  Finally, run the program.
## Startup
BeatVOX will boot utilizing your computer's default audio input and output devices.  These devices can be changed via pressing the "Settings" button in the top right corner of the application.  Within this settings menu, parameters such as the sample rate, buffer size, and MIDI output device can be modified as well.  By default, the MIDI output device is undefined, due to discrepancies between operating systems, so be sure to manually choose an available output to transmit your MIDI information.  Instructions on setting up a virtual MIDI port are provided later in the document.

If the selected audio devices are undefined, incompatible, or unable to be opened, the application will provide an error message as well as disable the recording functionality until appropriate modifications are made in the settings menu.  Also, the application will not allow the transmission of MIDI information until a MIDI output device is selected.

## How To Use BeatVOX
After your inputs and outputs are initialized under the settings menu, you can configure the desired tempo and number of bars to record, as well as enable or disable the metronome during recording. To record, click "Record" and the app will capture audio for the selected amount of time. If you mess up in the middle of the recording process, click "Stop Recording" to prematurely stop the recording then "Reset" to reinitialize the recorder.  Similarly, if you are unhappy with your performance after the recording process has completed, simply press "Reset." Please note, pressing "Stop Recording" will still produce a valid, albeit shorter, recording that can be sent to the analysis if desired.  The GUI buttons will update to inform you whether the app is recording, finished, or ready to record again. Once satisfied with your recording, click "Analyze" to process your audio. Once the MIDI pattern has been generated, the "Play" and "Stop" buttons will enable, which can be used to start/stop streaming the MIDI information through the selected MIDI output.
  
## Virtual MIDI Port Setup
Here's a link to instructions for both MAC and Windows:  https://dialogaudio.com/modulationprocessor/guides/virtual_midi/virtual_midi_setup.php
 
## Things to be Aware of
Currently, BeatVOX only functions with sampling rates of 44100Hz and 48000Hz and can only process kick, snare, and hi-hat sounds.  During the MIDI conversion, a kick is mapped to the MIDI note 36, a snare is mapped to 38, and a hi-hat to 42.  This is still a prototype build, so please mind the relative inaccuracy of the drum conversion.

## Demo Video and Paper
Here is a link to a demo video as well as a detailed paper regarding BeatVOX's functionality and development:
https://drive.google.com/drive/folders/1mhuZa5Pbmh2EgYIQqRtFaXz7PlzA4K6L?usp=sharing

