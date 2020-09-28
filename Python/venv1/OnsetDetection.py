import numpy as np
import math
import matplotlib.pyplot as plt
from scipy import signal
import scipy as sp
import os
import librosa
from scipy.io.wavfile import read
import mido
import pandas as pd


#y, fs = librosa.load('/Users/walterkopacz/PycharmProjects/BeatVoxPrototype/venv1/BeatVoxTest1.wav', sr=44100)

#Compute novelty function based on spectral flux (log magnitude)
def compute_novelty_function(x, method):
    #normalize audio
    x = x/max(abs(x))
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
    return (peaks[0])

    end
#ground_truth = np.transpose(np.genfromtxt('/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset1_onsets.csv', delimiter=','))

#nov = compute_novelty_function(y, 'log-mag')
#peaks, yeah = pick_peaks(nov, .015)
#fig = plt.figure()
#fig.add_subplot(111)
#fig = plt.plot(nov)
#print(peaks)
#plt.show()

#Test onset detection using F-Score
#Arguments: List of audio file paths, ground truth onsets file path, method of onset detection, threshold for peak pickng
def test_onset_detection(audio, ground_truth, method, thres):
    #use pandas not np for test
    #need to get onsets in time - use librosa blocks to time(?)

    ground_truth = pd.read_csv(ground_truth)

    # create dictionary of onsets
    onset_dict = {}

    for file in audio:
        #load audio file
        y, fs  = librosa.core.load(file, sr=44100)
        #compute novelty function
        nov = compute_novelty_function(y, method)
        #pick peaks
        peaks = pick_peaks(nov, thres)
        #convert peak blocks to peak times
        onsets = librosa.core.blocks_to_time(peaks, block_length=2048, hop_length=512, sr=fs) /1000
        #add onsets to pd dict
        onset_dict[file] = onsets
    #mirex evaluation procedure
    onsets_df = pd.DataFrame(dict([ (k,pd.Series(v)) for k,v in onset_dict.items() ]))
    onsets_df.columns = ground_truth.columns
    #Number of correctly detected onsets
    Ocd = 0
    #Number of false negatives - tolerance windows without an onset present
    Ofn = 0
    #Number of false positives - number of detections outside of all tolerance windows, number of extra detections within a specific tolerance window
    #0fp is the total detected onsets - Ocd
    Ofp = 0

    #Find numbers for each file
    for col in onsets_df.columns:
        #get column for corresponding audio file in ground truth onsets
        gt_df = ground_truth.loc[:, col]
        #drop all nan values
        gt_df = gt_df.dropna(how=any)
        #get column for corresponding detected onsets
        det_df = onsets_df.loc[:, col]
        for gt_onset in gt_df[:]:
            #get number of values within the threshold
            trues = det_df[(det_df >= gt_onset-.05) & (det_df <= gt_onset+.05)]
            if len(trues) == 1:
                Ocd += 1
            elif len(trues) == 0:
                Ofn += 1
            else:
                Ocd += 1
                Ofp += len(trues) - 1

    #Compute precision, recall, and F score
    precision = Ocd / (Ocd + Ofp)
    recall =  Ocd / (Ocd + Ofn)
    fmeasure = 2*precision*recall/(precision+recall)

    return (precision, recall, fmeasure)
    end


audio_paths = ['/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/snare_hex.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_william.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_vonny.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_pepouni.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_dbztenkaichi.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_bui.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_Turn-Table.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_Pneumatic.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_mouss.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_mcld.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_luckeymonkey.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_azeem.wav' , '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_adiao.wav' , '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/battleclip_daq.wav']
ground_truth_path = '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset1_onsets.csv'
print(test_onset_detection(audio_paths, ground_truth_path, 'log-mag', .015 ))

# not doing great so far: Look into: 1. how blocks_to_time might be affecting times, 2. the threshold















