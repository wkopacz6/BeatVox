import numpy as np
import math
import matplotlib.pyplot as plt
from scipy import signal
import scipy as sp
import os
import librosa
from scipy.io.wavfile import read
import mido


#Compute novelty function based on spectral flux
def compute_novelty_function():
    end