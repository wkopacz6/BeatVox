# BeatVox
## Here is a list of links to things I found that might be useful
http://www2.ece.rochester.edu/~zduan/teaching/ece477/projects/2017/Arvind_Ramanathan_ReportFinal.pdf
https://ieeexplore.ieee.org/abstract/document/7178773
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.440.3197&rep=rep1&type=pdf#page=143

## Onset Detection
Onset detection evaluated using MIREX 2018 methods: https://www.music-ir.org/mirex/wiki/2018:Audio_Onset_Detection#Evaluation_Procedures

----------------------------------------------------------------
## What is BeatVOX?
BeatVOX is an application that can convert an audio recording of beatboxing into a corresponding MIDI drum pattern, which can be streamed directly to your DAW or other MIDI-input applications.
  
## Required Files
RecorderAttempt_2.jucer (I'll change the name of this later), Main.cpp, MainComponent.cpp, MainComponent.h, Metronome.cpp, Metronome.h, classification.cpp, classification.h, onsetDetection.cpp, onsetDetection.h, classification.h, recordAudio.cpp, recordAudio.h, and Cowbell-2.wav

Please keep all .jucer, .h, and .cpp files in their original folder configuration and move the "Cowbell-2.wav" to your desktop.
  
## Startup
BeatVOX will boot utilizing your computer's default audio input and output devices.  These devices can be changed via pressing the "Settings" button in the top right corner of the application.  Within this settings menu, parameters such as the sample rate, buffer size, and MIDI output device can be modified as well.  By default, the MIDI output device is undefined, due to discrepancies between operating systems, so be sure to manually choose an available output to transmit your MIDI information.  Instructions on setting up a virtual MIDI port are provide later in the document.

If the selected audio devices are undefined, incompatible, or unable to be opened, the application will provide an error message as well as disable the recording functionality until appropriate modifications are made in the settings menu.  Also, the application will not allow the transmission of MIDI information until a MIDI output device is selected.

## How To Use BeatVOX
After your inputs and outputs are initialized under the settings menu, you can configure the desired tempo and number of bars to record, as well as enable or disable the metronome during recording. To record, click "Record" and the app will capture audio for the selected amount of time. If you mess up your recording, simply click "Reset" either during recording or after it has finished. The GUI buttons will update to inform you whether the app is recording, finished, or ready to record again. Once satisfied with your recording, click "Analyze" to process your audio. Once the MIDI is ready, the "Play" and "Stop" buttons will become available, which can be used to start/stop streaming the MIDI information to the selected MIDI output.
  
## Virtual MIDI Port Setup
Here's a link to instructions for both MAC and Windows:  https://dialogaudio.com/modulationprocessor/guides/virtual_midi/virtual_midi_setup.php
 
## Things to be Aware of
If you are using the metronome, be sure to use headphones to prevent bleed and possible errors with the classification.
Make sure you do your best to provide a clear recording to the app, i.e.(minimal background noise, keep a consistent distance from the microphone)

