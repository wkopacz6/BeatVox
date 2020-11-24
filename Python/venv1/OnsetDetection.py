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




#Compute novelty function based on spectral flux (log magnitude)
def compute_novelty_function(x, method, nfft, hoplength):
    #normalize audio
    max1 = max(abs(x))
    x = x/max1
    # use librosa to calculate the stft
    X = librosa.core.stft(x, n_fft=nfft, hop_length=hoplength, center=False)
    X = abs(X)
    if method == 'log-mag':

        #compute the log-magnitude spectrum
        #(issue 1) add 1e-12 to the magnitude spectrum to ensure no zero values
        log_mag_X = 20*np.log10(np.abs(X)+1e-12)
        log_mag_X = np.c_[log_mag_X[:, 0], log_mag_X]

        diff_log_mag_X = np.diff(log_mag_X, 1, axis=1)
        #(issue 2) Half-wave rectification
        diff_log_mag_X[diff_log_mag_X < 0] = 0

        # RMS of diff
        flux = np.sqrt((diff_log_mag_X**2).sum(axis=0) / log_mag_X.shape[0])
        flux = flux/ max(abs(flux))


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




#moving average filter used in peak picking
def moving_average(a, n) :
    a = np.concatenate((np.zeros(n-1), a))
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def pick_peaks(nov, thres, nfft, hoplength, fs):
    #determine adaptive threshold
    #take the moving average of the novelty function
    #(issue 4) use as adaptive threshold

    #Note: Adaptive threshold w/ moving average: F=.887, accuracy=.97, recall=.82
    #Note: Adaptive threshold w/ median filter: F=.895, acccuracy=.96, recall=.84

    #nov_thres = moving_average(nov, n=5)
    nov_thres = sp.signal.medfilt(nov, 5)

    #use scipy find peaks
    peaks = sp.signal.find_peaks(nov, height=nov_thres+thres, distance=4)
    # convert peak blocks to peak times
    # (issue 3)
    onsets = peaks[0] * hoplength / fs
    return (onsets, peaks, nov_thres)

    end


def output_segments(audio, onsets_in_seconds, fs, output_number_of_samples):
    onsets_in_samples = np.floor(onsets_in_seconds * fs)
    i = 0
    audio_segments = np.zeros((output_number_of_samples, len(onsets_in_seconds)))
    for onset in onsets_in_samples:
        onset = int(onset)
        audio_segments[:, i] = audio[onset:onset+output_number_of_samples]
        i += 1
    return audio_segments


#Test onset detection using F-Score
#Arguments: List of audio file paths, ground truth onsets file path, method of onset detection, threshold for peak pickng
def test_onset_detection(audio, ground_truth, method, thres, nfft, hoplength, fs):
    #use pandas not np for test
    #need to get onsets in time - use librosa blocks to time(?)

    ground_truth = pd.read_csv(ground_truth)

    # create dictionary of onsets
    onset_dict = {}

    for file in audio:
        #load audio file
        y, fs  = librosa.core.load(file, sr=fs)
        #compute novelty function
        nov = compute_novelty_function(y, method, nfft=nfft, hoplength=hoplength)
        #pick peaks
        peaks, unused, unused2 = pick_peaks(nov, thres, nfft=nfft, hoplength=hoplength, fs=fs)

        #add onsets to pd dict
        onset_dict[file] = peaks
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
    fmeasure = (2*precision*recall)/(precision+recall)

    return (precision, recall, fmeasure)
    end


#audio_paths_analysis = ['/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/snare_hex.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_william.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_vonny.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_pepouni.wav']
#audio_paths_test = ['/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_dbztenkaichi.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/putfile_bui.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_Turn-Table.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_Pneumatic.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_mouss.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_mcld.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_luckeymonkey.wav', '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_azeem.wav' , '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/callout_adiao.wav' , '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/battleclip_daq.wav']

#ground_truth_path_analysis = '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset1_onsets.csv'
#ground_truth_path_test = '/Users/walterkopacz/Documents/GitHub/BeatVox/Python/beatboxset1/beatboxset1_annotations/beatboxset2_onsets.csv'



#print(test_onset_detection(audio_paths_test, ground_truth_path_test, 'log-mag', 0, nfft=1024, hoplength=768, fs=44100))


recorded_audio = pd.read_csv('/Users/walterkopacz/Documents/Test_Audio_Recording.csv')
JUCE_onsets = pd.read_csv('/Users/walterkopacz/Documents/Test_Segmentation.csv')

recorded_audio_np = recorded_audio.to_numpy()
JUCE_onsets = JUCE_onsets.to_numpy()
novelty_func = compute_novelty_function(recorded_audio_np[:, 0], 'log-mag', 1024, 768)
pks = pick_peaks(novelty_func, .2, 1024, 768, 44100)[0]


dif = np.c_[np.transpose(novelty_func), JUCE_onsets[:, 0]]
diff_nov = np.diff(dif, axis=1)
diff_peaks = np.diff(pks, JUCE_onsets.loc[:, 1])















