# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import os

import librosa as librosa
import numpy as np
import pandas as pd
from libsvm.svm import svm_problem, svm_parameter
from libsvm.svmutil import *

# normalize audio file function
def normalize(audio_sample):
    return audio_sample / max(abs(audio_sample))

def min_max_normalization(x):
    x = np.asarray(x)
    min_val = np.amin(np.absolute(x))
    max_val = np.amax(np.absolute(x))
    x_scaled = (x - min_val)/(max_val-min_val)
    return x_scaled

def test_classification(audio_path, ground_truth_path):
    # this code counts the total number of beats in a given folder of onset files
    os.chdir(ground_truth_path)
    csv_name = os.listdir()
    total_beat_count = 0
    starting_index = np.array([])

    for onset_file in csv_name:
        current_csv = pd.read_csv(onset_file)
        row, col = current_csv.shape
        total_beat_count = total_beat_count + row

    # initialize empty feature vector (size = total number of beats * mfcc dimension)
    features = np.empty([total_beat_count, 15])
    label = np.empty([total_beat_count])

    os.chdir(audio_path)
    file_name = os.listdir()
    print(file_name)
    # the beat index from count 0 across multiple audio files in beatboxset1
    index = 0


    for audio in file_name:
        starting_index = np.append(starting_index, np.array(int(index)))
        y, sr = librosa.core.load(audio)
        y = normalize(y)
        os.chdir(ground_truth_path)
        current_onset = pd.read_csv(audio[0:-3] + 'csv')
        row, col = current_onset.shape

        for x in range(row):
            if current_onset['Type'][x] == 'k':
                label[index] = 1
            elif current_onset['Type'][x] == 'hc' or current_onset['Type'][x] == 'ho':
                label[index] = 2
            # elif current_onset['Type'][x] == 'ho':
            #     label[index] = 3
            elif current_onset['Type'][x] == 'sb' or current_onset['Type'][x] == 'sk' or current_onset['Type'][x] == 's':
                label[index] = 4
            # elif current_onset['Type'][x] == 'sk':
            #     label[index] = 5
            # elif current_onset['Type'][x] == 's':
            #     label[index] = 6
            # elif current_onset['Type'][x] == 'br':
            #     label[index] = 7
            # elif current_onset['Type'][x] == 'm':
            #     label[index] = 8
            # elif current_onset['Type'][x] == 'v':
            #     label[index] = 9
            # elif current_onset['Type'][x] == 'x':
            #     label[index] = 10
            else:
                label[index] = 11

            print(str(current_onset['Timestamp'][x]) + ' ' + audio + ' ' + str(x))
            start_sample = int(sr * current_onset['Timestamp'][x])

            # if this beat is the last beat of the track
            if x == row - 1:
                end_sample = len(y) - 1
            else:
                end_sample = int(sr * current_onset['Timestamp'][x + 1])
            single_beat = y[start_sample: end_sample: 1]

            # if fft window is larger than the total samples of the beat, pad zeros to both left and right sides of the signal
            n_fft_window = 2048
            if n_fft_window > single_beat.shape[-1]:
                single_beat = np.pad(single_beat, int(n_fft_window // 2), mode='reflect')

            mfccs = librosa.feature.mfcc(y=single_beat, n_mfcc=15, n_fft=n_fft_window)
            feature_vector = np.mean(mfccs, axis=1)
            features[index, :] = feature_vector
            index = index + 1

            os.chdir(audio_path)

    # for i in range(len(starting_index)):
    #     test_features = features[starting_index[i], starting_index[i+1]]
    #     train_features = features[0:starting_index-1]
    #features = min_max_normalization(features)
    print(index)
    print(features.shape)
    print(label)
    #print(starting_index)

    c_val = np.array([2**-5, 2**-3, 2**-1, 2**1, 2**3, 2**5, 2**7, 2**9, 2**11, 2**13, 2**15])
    gamma = np.array([2**-15, 2**-13, 2**-11, 2**-9, 2**-7, 2**-5, 2**-3, 2**-1, 2**1, 2**3])
    acc = np.empty([len(c_val), len(gamma)])
    for p in range(len(c_val)):
        for k in range(len(gamma)):
            prob = svm_problem(label[1:-400:1], min_max_normalization(features[1:-400:1]))
            param = svm_parameter('-t 0 -c ' + str(c_val[p]) + ' ' + '-g ' + str(gamma[k]))
            model = svm_train(prob, param)
            p_label, p_acc, p_val = svm_predict(label[-400:], min_max_normalization(features[-400:]), model)
            acc[p,k] = p_acc[0]
    print(acc)
    print('FINISHED')




audio_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\beatboxset1'
ground_truth_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\annotation'
test_classification(audio_path_test, ground_truth_path_test)

