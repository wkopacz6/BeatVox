# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import os

import librosa
from librosa.feature import zero_crossing_rate
import matplotlib.pyplot as plt
import numpy as np

basepath = r'testing'
list = os.listdir(basepath) # dir is your directory path
file_count = len(list)
plt.figure(figsize=(14, 8))
with os.scandir(basepath) as entries:
    cur_file = 0
    for entry in entries:
        if entry.is_file():
            cur_file = cur_file+1
            print(entry.name)
            [y, sr] = librosa.load(entry)
            zcr = zero_crossing_rate(y.astype(float))
            cent = librosa.feature.spectral_centroid(y.astype(float),n_fft=100,hop_length=50)
            flatness = librosa.feature.spectral_flatness(y.astype(float),n_fft=100,hop_length=50)
            print(flatness)

            x = np.linspace(0, len(y)/sr, len(zcr[0]))
            plt.subplot(file_count, 3, cur_file*3-2)
            plt.plot(x,np.transpose(zcr))
            plt.ylim(0, 0.3)
            plt.title(entry.name)
            plt.subplot(file_count, 3, cur_file*3-1)
            plt.semilogy(cent[0])
            plt.title('Spectral centroid ' + entry.name)
            plt.ylim(1000, 5000)
            plt.ylabel('Hz')
            plt.subplot(file_count, 3, cur_file*3)
            plt.plot(flatness[0])
            plt.ylim(0, 0.1)



plt.show()








def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.




# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')
    print_hi('Joshua');

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
