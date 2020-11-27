import librosa as librosa
import numpy as np
import pandas as pd
from libsvm.commonutil import csr_find_scale_param, csr_scale
from libsvm.svm import svm_problem, svm_parameter
from libsvm.svmutil import *
import os

from numpy import random


def normalize(audio_sample):
    return audio_sample / max(abs(audio_sample))

def normalize_testing(test, min_vec, max_vec):
    test_scaled = (test - min_vec) / (max_vec - min_vec)
    return test_scaled


def test(audio_path, ground_truth_path):
    # this code counts the total number of beats in a given folder of onset files
    os.chdir(ground_truth_path)
    csv_name = os.listdir()
    total_beat_count = 0


    for onset_file in csv_name:
        current_csv = pd.read_csv(onset_file)
        row, col = current_csv.shape
        total_beat_count = total_beat_count + row

    # initialize empty feature vector (size = total number of beats * mfcc dimension)
    features = np.empty([297, 18])
    label = np.empty([297])

    os.chdir(audio_path)
    file_name = os.listdir()
    print(file_name)
    # the beat index from count 0 across multiple audio files in beatboxset1
    index = 0
    fsamp = 44100


    print('start parsing ' + 'snare_hex')
    audio = 'callout_azeem.wav'
    y, sr = librosa.core.load(audio, sr=fsamp)
    y = normalize(y)
    os.chdir(ground_truth_path)
    current_onset = pd.read_csv(audio[0:-3] + 'csv')
    row, col = current_onset.shape

    for x in range(row):

        if current_onset['Type'][x] == 'k':
            label[index] = 36
        elif current_onset['Type'][x] == 'hc' or current_onset['Type'][x] == 'ho':
            label[index] = 38
        # elif current_onset['Type'][x] == 'ho':
        #     label[index] = 3
        elif current_onset['Type'][x] == 'sb' or current_onset['Type'][x] == 'sk' or current_onset['Type'][
            x] == 's':
            label[index] = 42
        else:
            label[index] = np.random.choice([36, 38, 42],1)


        print(str(current_onset['Timestamp'][x]) + ' ' + audio + ' ' + current_onset['Type'][x] + ' ' + str(x) + ' ' + str(index))
        start_sample = int(sr * current_onset['Timestamp'][x])

        # if this beat is the last beat of the track, set it to 0.25 seconds
        if x == row - 1:
            end_sample = start_sample + int(sr * 0.04)
        else:
            end_sample = int(start_sample + 0.04*sr)
            #end_sample = int((sr * current_onset['Timestamp'][x + 1] - start_sample)*0.2) + start_sample
        single_beat = y[start_sample: end_sample: 1]

        # if fft window is larger than the total samples of the beat, pad zeros to both left and right sides of the signal
        n_fft_window = 1024
        hop_size = 768

        mfccs = librosa.feature.mfcc(y=single_beat, n_mfcc=18, n_fft=n_fft_window, hop_length = hop_size, sr=sr)
        feature_vector = np.mean(mfccs, axis=1)
        features[index,:] = feature_vector

        index = index + 1



        os.chdir(audio_path)


    print(len(label))
    print(len(features))

    os.chdir(r'C:\Users\Joshua\Documents\GitHub\3770python')
    model = svm_load_model("model_file44100")
    vec1_48000 = np.array([-656.27661133, -23.91659355, -156.65887451, -26.94819069, -56.11130905, -55.74367142, -53.92170715,
            -53.11208725, -53.85018539, -34.81391525, -35.0021019, -21.57038116, -26.25063133, -37.53669357,
            -30.06274223, -19.2313633, -30.5000782, -21.15400314])
    vec2_48000 = np.array([-36.69165039, 252.31211853, 44.37088013, 133.45358276, 70.93629456, 79.21143341, 74.84476471, 49.84890747,
            40.43525314, 46.55800247, 32.27348709, 34.62980652, 18.92416573, 29.59581566, 28.95998383, 25.31793213,
            25.91612244, 23.99448586])
    vec1_44100 = np.array([-648.47772217, -41.23974609, -152.00854492, -23.61817169, -61.95689011, -38.56236649, -37.30766678, -67.04342651,
     -40.2208519, -41.70504379, -31.27592278, -24.66895866, -28.49080849, -37.02992249, -32.1645546, -24.17269897,
     -21.82624054, -20.05064392])
    vec2_44100 = np.array([-23.18715858, 237.5743866, 44.8717041, 132.29042053, 69.93689728, 76.80852509, 70.79082489, 48.25970459, 41.93119049, 37.43262863, 36.47388077, 39.49528122, 21.26286507, 28.64596558, 33.27782822, 31.86940575, 21.46536827, 20.09148598])

    normalized_mfcc = normalize_testing(features, vec1_44100, vec2_44100)
    outputLabel = svm_predict(label, normalized_mfcc, model)
    outputLabel = np.transpose(np.array(outputLabel[0]))
    os.chdir(r'C:\Users\Joshua\Documents\GitHub\BeatVox\3770python')
    outputLabel.tofile("callout_azeem_test.csv", sep=',')
    print('Finished')





audio_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\beatboxset1'
ground_truth_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\annotation'
test(audio_path_test, ground_truth_path_test)
