# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

from scipy.sparse import csr_matrix
import librosa as librosa
import numpy as np
import pandas as pd
from libsvm.commonutil import csr_find_scale_param, csr_scale
from libsvm.svm import svm_problem, svm_parameter
from libsvm.svmutil import *
import os

# normalize audio file function
def normalize(audio_sample):
    return audio_sample / max(abs(audio_sample))


def normalize_training(train):
    train = np.asarray(train)
    min_val = np.amin(train, axis=0)
    max_val = np.amax(train, axis=0)
    train_scaled = (train - min_val) / (max_val - min_val)
    return train_scaled, min_val, max_val


def normalize_testing(test, min_vec, max_vec):
    test_scaled = (test - min_vec) / (max_vec - min_vec)
    return test_scaled


def test_classification(audio_path, ground_truth_path):
    # this code counts the total number of beats in a given folder of onset files
    os.chdir(ground_truth_path)
    csv_name = os.listdir()
    total_beat_count = 0


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
        print('start parsing ' + audio)
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
            elif current_onset['Type'][x] == 'sb' or current_onset['Type'][x] == 'sk' or current_onset['Type'][
                x] == 's':
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

            print(str(current_onset['Timestamp'][x]) + ' ' + audio + ' ' + current_onset['Type'][x] + ' ' + str(x) + ' ' + str(index))
            start_sample = int(sr * current_onset['Timestamp'][x])

            # if this beat is the last beat of the track, set it to 0.25 seconds
            if x == row - 1:
                end_sample = start_sample + int(sr * 0.25)
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


    print(index)
    print(features.shape)
    print(label)
    # train_scaled, vec1, vec2 = normalize_training(features[1:-400:1])
    # test_scaled = normalize_testing(features[-400:], vec1, vec2)
    # pd.DataFrame(data=train_scaled).to_csv(r'C:\Users\Joshua\Desktop\MUSI 3770\train.csv', index = False, header = False)
    # pd.DataFrame(data=label).to_csv(r'C:\Users\Joshua\Desktop\MUSI 3770\label.csv', index = False, header = False)
    # pd.DataFrame(data=test_scaled).to_csv(r'C:\Users\Joshua\Desktop\MUSI 3770\test.csv', index=False, header=
    acc = np.empty(10)
    for r in range(10):
        train_scaled, vec1, vec2 = normalize_training(np.concatenate((features[0:384*r:1], features[384*r+384::1])))
        prob = svm_problem(np.concatenate((label[0:384*r:1], label[384*r+384::1])), train_scaled)
        param = svm_parameter('-t 0 -c 100')
        model = svm_train(prob, param)
        p_label, p_acc, p_val = svm_predict(label[384*r:384*r + 384:1], normalize_testing(features[384*r:384*r + 384:1], vec1, vec2), model)
        acc[r] = p_acc[0]
    # still not to the point of testing c_val and gamma
    # c_val = np.array([2**-5, 2**-3, 2**-1, 2**1, 2**3, 2**5, 2**7, 2**9, 2**11, 2**13, 2**15])
    # gamma = np.array([2**-15, 2**-13, 2**-11, 2**-9, 2**-7, 2**-5, 2**-3, 2**-1, 2**1, 2**3])
    # acc = np.empty([len(c_val), len(gamma)])
    # for p in range(len(c_val)):
    #     for k in range(len(gamma)):
    #         prob = svm_problem(label[1:-400:1], min_max_normalization(features[1:-400:1]))
    #         param = svm_parameter('-t 0 -c ' + str(c_val[p]) + ' ' + '-g ' + str(gamma[k]))
    #         model = svm_train(prob, param)
    #         p_label, p_acc, p_val = svm_predict(label[-400:], min_max_normalization(features[-400:]), model)
    #         acc[p,k] = p_acc[0]
    # print(acc)
    print('FINISHED')


audio_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\beatboxset1'
ground_truth_path_test = r'C:\Users\Joshua\Documents\GitHub\3770python\annotation'
test_classification(audio_path_test, ground_truth_path_test)


