# BeatVox
## Here is a list of links to things I found that might be useful
http://www2.ece.rochester.edu/~zduan/teaching/ece477/projects/2017/Arvind_Ramanathan_ReportFinal.pdf
https://ieeexplore.ieee.org/abstract/document/7178773
http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.440.3197&rep=rep1&type=pdf#page=143

## Onset Detection
Onset detection evaluated using MIREX 2018 methods: https://www.music-ir.org/mirex/wiki/2018:Audio_Onset_Detection#Evaluation_Procedures

----------------------------------------------------------------

# Startup
BeatVOX will boot utilizing your computer's default audio input and output devices.  These devices can be changed via pressing the "Settings" button in the top right corner of the application.  Within this settings menu, parameters such as the sample rate, buffer size, and MIDI output device can be modified as well.  By default, the MIDI output device is undefined, due to discrepancies between operating systems, so be sure to manually choose an available output to transmit your MIDI information.  Instructions on setting up a virtual MIDI port are provide later in the document.

If the selected audio devices are undefined, incompatible, or unable to be opened, the application will provide an error message as well as disable the recording functionality until appropriate modifications are made in the settings menu.  Also, the application will not allow the transmission of MIDI information until a MIDI output device is selected.
