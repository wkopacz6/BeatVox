from main import normalize_training, normalize_testing
import librosa
import numpy as np
from libsvm.svm import svm_problem, svm_parameter
from libsvm.svmutil import *
from main import vec1, vec2

fs = 44100
n_fft_window = 2048
param = svm_parameter('-t 0 -c 100')
model = svm_load_model('model_file')
def demo(segment):
    if len(segment) < 0.04 * fs:
        np.pad(segment, int(0.04 * fs // 2), mode='reflect')
    else:
        segment = segment[0:int(0.04 * fs)]
    if n_fft_window > segment.shape[-1]:
        segment = np.pad(segment, int(n_fft_window // 2), mode='reflect')
    mfccs = librosa.feature.mfcc(y=segment, n_mfcc=18, n_fft=n_fft_window)
    feature_vector = np.mean(mfccs, axis=1)
    p_label, p_acc, p_val = svm_predict([], normalize_testing(feature_vector, vec1, vec2), model)
    return p_label

