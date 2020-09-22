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


nov = compute_novelty_function(y, 'laroche')
fig = plt.figure()
fig.add_subplot(111)
fig = plt.plot(nov)
plt.show()