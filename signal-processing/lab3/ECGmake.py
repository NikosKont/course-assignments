import numpy as np
import math as m
import scipy.io as io
import scipy.signal as sig
import matplotlib.pyplot as plt
import scipy.special as sp
import sys


# ecgsig = vector of signal samples at fs samples/sec
# fint = frequency of the interfering sinusoid (near 50 Hz)
# rnstring = your 4-digit CSD/MATH/MED/PHYS serial number as a string,
# e.g., ’1367’ or ’3646’ or '1040150'

def ECGmake(rnstring):
    if (len(rnstring) < 4) or (len(rnstring) >= 8):
        sys.exit("Your S/N is invalid!")

    x = int(rnstring)

    if not x:
        sys.exit("Your S/N is invalid!")

    y = x*(10**(-3))

    f0 = 50
    w0 = ((round(f0*y*100)/100) + np.random.rand(1, 1)) / (((round(y)*100)/100) - 0.5*np.random.rand(1, 1))

    # Load ECG signal
    m = io.loadmat('ECG.mat')
    ecgsig = sig.resample(m['fileECG'], int(len(m['fileECG']) * 8000 / m['fs'][0]))
    fs = 8000
    ecgsig = ecgsig[0:len(ecgsig)-500]
    ecgsig = ecgsig/max(abs(ecgsig))
    ecgsig = ecgsig - np.mean(ecgsig)

    # Add interference
    ecgsig = ecgsig + 0.05 * np.cos(2 * np.pi * w0 / fs * np.arange(1, len(ecgsig) + 1)).T

    fint = w0
    return ecgsig, fs, fint
# ecgsig, fs, fint

