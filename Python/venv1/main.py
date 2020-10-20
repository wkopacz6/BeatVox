import numpy as np
import math
import matplotlib.pyplot as plt
from scipy import signal
import scipy as sp
import os
import librosa
from scipy.io.wavfile import read
import mido
from Python.venv1.OnsetDetection import compute_novelty_function, pick_peaks, output_segments

filename = '//Users/walterkopacz/Documents/GitHub/BeatVox/Python/venv1/BeatVoxTest1#04.wav'
hoplen = 512
nfft = 2048

#load in audio
y1, fs = librosa.load(filename, sr=44100)

#Onset detection
nov = compute_novelty_function(y1, 'log-mag', 1024, 768)
onsets, peaks, thres = pick_peaks(nov, .2, 1024, 768)

segs = output_segments(audio=y1, onsets_in_seconds=onsets, fs=fs, output_number_of_samples=1024)

#Timbre Detection
cent = np.transpose(librosa.feature.spectral_centroid(y1, sr=fs, n_fft=nfft))
#only look at SC at onsets
cent = cent[peaks[0]]
#If SC is above 6000 snare; if below 2500 kick drum3
fig = plt.figure()
fig.add_subplot(111)
fig = plt.plot(nov)
fig = plt.plot(thres)
plt.show()
#MIDI Creation

port = mido.open_output('IAC Driver Bus 1')
mid = mido.MidiFile()
track = mido.MidiTrack()
mid.tracks.append(track)

i = 0
for ons in onsets:
    if i == 0:
        t = int(mido.second2tick(onsets[i], ticks_per_beat=mid.ticks_per_beat, tempo=500000))
    else:
        t = int(mido.second2tick(onsets[i] - onsets[i - 1], ticks_per_beat=mid.ticks_per_beat, tempo=500000))

    msg = mido.Message('note_on', note=36, time=t)
    track.append(msg)
    i += 1

mid.save('beat.mid')

for msg1 in mido.MidiFile('beat.mid').play():
    port.send(msg1)
    print(msg1)









