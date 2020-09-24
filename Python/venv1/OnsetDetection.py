import numpy as np
import math
import matplotlib.pyplot as plt
from scipy import signal
import scipy as sp
import os
import librosa
from scipy.io.wavfile import read
import mido


y, fs = librosa.load('/Users/walterkopacz/PycharmProjects/BeatVoxPrototype/venv1/BeatVoxTest1.wav', sr=44100)

#Compute novelty function based on spectral flux (log magnitude)
def compute_novelty_function(x, method):
    # use librosa to calculate the stft
    X = librosa.core.stft(x, n_fft=2048)
    if method == 'log-mag':



        #compute the log-magnitude spectrum
        log_mag_X = np.abs(X)
        log_mag_X = np.c_[log_mag_X[:, 0], np.log10(log_mag_X)]

        diff_log_mag_X = np.diff(log_mag_X, 1, axis=1)

        # RMS of diff
        flux = np.sqrt((diff_log_mag_X ** 2).sum(axis=0)) / log_mag_X.shape[0]

        return (flux)

    elif method == 'hains':
        epsilon = 1e-5

        # difference spectrum (set first diff to zero)
        X = np.c_[X[:, 0], np.sqrt(X)]
        X[X <= 0] = epsilon

        afDiff = np.log2(X[:, np.arange(1, X.shape[1])] / X[:, np.arange(0, X.shape[1] - 1)])

        # flux
        d_hai = np.sum(afDiff, axis=0) / X.shape[0]

        return (d_hai)
    elif method == 'laroche':
        # difference spectrum (set first diff to zero)
        X = np.c_[X[:, 0], np.sqrt(X)]

        afDiff = np.diff(X, 1, axis=1)

        # flux
        d_lar = np.sum(afDiff, axis=0) / X.shape[0]

        return (d_lar)





    end

#moving average filter used in peak picking
def moving_average(a, n=3) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def pick_peaks(nov, thres):
    #determine adaptive threshold
    #take the moving average of the novelty function
    nov = moving_average(nov, n=5)

    #use scipy find peaks
    peaks = sp.signal.find_peaks(nov, height=thres)
    return (peaks)

    end
ground_truth = np.transpose(np.genfromtxt('/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset1_onsets.csv', delimiter=','))

nov = compute_novelty_function(y, 'log-mag')
#peaks, yeah = pick_peaks(nov, .015)
#fig = plt.figure()
#fig.add_subplot(111)
#fig = plt.plot(nov)
#print(peaks)
#plt.show()

#Test onset detection using F-Score
def test_onset_detection():
    #Compute novelty function
    #use pandas not np for test
    #need to get onsets in time 

    ground_truth = np.transpose(np.genfromtxt('/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset1_onsets.csv', delimiter=','))


    end
