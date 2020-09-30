import numpy as np
import math
import matplotlib.pyplot as plt
from scipy import signal
import scipy as sp
import os
import librosa
from scipy.io.wavfile import read
import mido


filename = '/Users/walterkopacz/PycharmProjects/BeatVoxPrototype/venv1/BeatVoxTest1.wav'
hoplen = 512
nfft = 2048

#load in audio
y1, fs = librosa.load(filename, sr=44100)

#Onset detection
def compute_novelty_function(x, method, end=None):
    #normalize audio
    x = x/max(abs(x))
    # use librosa to calculate the stft
    X = librosa.core.stft(x, n_fft=1024, hop_length=768)
    if method == 'log-mag':

        #compute the log-magnitude spectrum
        log_mag_X = 20*np.log10(np.abs(X))
        log_mag_X = np.c_[log_mag_X[:, 0], log_mag_X]

        diff_log_mag_X = np.diff(log_mag_X, 1, axis=1)

        # RMS of diff
        flux = (diff_log_mag_X).sum(axis=0) / log_mag_X.shape[0]

        return (flux)
    #Directly from pyACA
    elif method == 'hains':
        epsilon = 1e-5

        # difference spectrum (set first diff to zero)
        X = np.c_[X[:, 0], np.sqrt(X)]
        X[X <= 0] = epsilon

        afDiff = np.log2(X[:, np.arange(1, X.shape[1])] / X[:, np.arange(0, X.shape[1] - 1)])

        # flux
        d_hai = np.sum(afDiff, axis=0) / X.shape[0]

        return (d_hai)
    #Directly from pyACA
    elif method == 'laroche':
        # difference spectrum (set first diff to zero)
        X = np.c_[X[:, 0], np.sqrt(X)]

        afDiff = np.diff(X, 1, axis=1)

        # flux
        d_lar = np.sum(afDiff, axis=0) / X.shape[0]

        return (d_lar)

    end

#moving average filter used in peak picking
def moving_average(a, n) :
    a = np.concatenate((np.zeros(n-1), a))
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def pick_peaks(nov, thres):
    #determine adaptive threshold
    #take the moving average of the novelty function
    nov = moving_average(nov, n=2)

    #use scipy find peaks
    peaks = sp.signal.find_peaks(nov, height=thres)
    # convert peak blocks to peak times
    onsets = librosa.core.frames_to_time(peaks[0], n_fft=1024, hop_length=768, sr=44100)
    return (peaks[0], onsets)

    end


nov = compute_novelty_function(y1, 'log-mag', end=None)
onsets, onsets_time = pick_peaks(nov, .28)


#Timbre Detection
cent = np.transpose(librosa.feature.spectral_centroid(y1, sr=fs, n_fft=nfft))
#only look at SC at onsets
cent = cent[onsets]
#If SC is above 6000 snare; if below 2500 kick drum3
#fig = plt.figure()
#fig.add_subplot(111)
#fig = plt.plot(cent)
#plt.show()
#MIDI Creation

port = mido.open_output('IAC Driver Bus 1')
mid = mido.MidiFile()
track = mido.MidiTrack()
mid.tracks.append(track)

i = 0
for ons in onsets:
    if i == 0:
        t = int(mido.second2tick(onsets_time[i], ticks_per_beat=mid.ticks_per_beat, tempo=500000))
    else:
        t = int(mido.second2tick(onsets_time[i] - onsets_time[i - 1], ticks_per_beat=mid.ticks_per_beat, tempo=500000))

    if cent[i] >= 6000:
        msg = mido.Message('note_on', note=36, time=t)
    if cent[i] <= 2500:
        msg = mido.Message('note_on', note=37, time=t)
    track.append(msg)
    i += 1

mid.save('beat.mid')

for msg1 in mido.MidiFile('beat.mid').play():
    port.send(msg1)
    print(msg1)









