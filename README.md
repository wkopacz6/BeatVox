# BeatVox
## Here is a list of links to things I found that might be useful
http://www2.ece.rochester.edu/~zduan/teaching/ece477/projects/2017/Arvind_Ramanathan_ReportFinal.pdf
https://ieeexplore.ieee.org/abstract/document/7178773
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.440.3197&rep=rep1&type=pdf#page=143

## Onset Detection
Onset detection evaluated using MIREX 2018 methods: https://www.music-ir.org/mirex/wiki/2018:Audio_Onset_Detection#Evaluation_Procedures

----------------------------------------------------------------
## What is BeatVOX?
BeatVOX is an application that can convert an audio recording of beatboxing into a corresponding MIDI drum pattern, which can be streamed directly into your DAW or to any other MIDI application of your choosing.
  
## Required Files
For BeatVOX to function properly, please ensure the following files are present:  BeatVOX.jucer, Main.cpp, MainComponent.cpp, MainComponent.h, Metronome.cpp, Metronome.h, classifyAudio.cpp, classifyAudio.h, onsetDetection.cpp, onsetDetection.h, recordAudio.cpp, recordAudio.h, svm.h, svm.cpp, Cowbell-2.wav, model_file44100, and model_file48000.

Please place downloaded files in your computer's Documents directory.  If you insist to place the files somewhere else, please at least keep the "Cowbell_2.wav" file in the Documents directory.  Keep files in their original folder layout.
  
## Startup
BeatVOX will boot utilizing your computer's default audio input and output devices.  These devices can be changed via pressing the "Settings" button in the top right corner of the application.  Within this settings menu, parameters such as the sample rate, buffer size, and MIDI output device can be modified as well.  By default, the MIDI output device is undefined, due to discrepancies between operating systems, so be sure to manually choose an available output to transmit your MIDI information.  Instructions on setting up a virtual MIDI port are provide later in the document.

If the selected audio devices are undefined, incompatible, or unable to be opened, the application will provide an error message as well as disable the recording functionality until appropriate modifications are made in the settings menu.  Also, the application will not allow the transmission of MIDI information until a MIDI output device is selected.

## How To Use BeatVOX
After your inputs and outputs are initialized under the settings menu, you can configure the desired tempo and number of bars to record, as well as enable or disable the metronome during recording. To record, click "Record" and the app will capture audio for the selected amount of time. If you mess up in the middle of the recording process, click "Stop Recording" to prematurely stop the recording then "Reset" to reinitialize the recorder.  Similarly, if you are unhappy with your performance after the recording process has completed, simply press "Reset." Please note, pressing "Stop Recording" will still produce a valid, albeit shorter, recording that can be sent to the analysis if desired.  The GUI buttons will update to inform you whether the app is recording, finished, or ready to record again. Once satisfied with your recording, click "Analyze" to process your audio. Once the MIDI pattern has been generated, the "Play" and "Stop" buttons will enable, which can be used to start/stop streaming the MIDI information to the selected MIDI output.
  
## Virtual MIDI Port Setup
Here's a link to instructions for both MAC and Windows:  https://dialogaudio.com/modulationprocessor/guides/virtual_midi/virtual_midi_setup.php
 
## Things to be Aware of
Currently, BeatVOX only functions with sampling rates of 44100Hz and 48000Hz and can only process kick, snare, and hi-hat sounds.  During the MIDI conversion, a kick is mapped to the MIDI note 36, a snare is mapped to 38, and a hi-hat to 42.  This is still a prototype build, so please mind the relative inaccuracy of the drum conversion.

