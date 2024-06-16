import numpy as np
import scipy.signal as sig
import matplotlib.pyplot as plt
import warnings
from matplotlib import patches
from matplotlib.colors import LogNorm
from mpl_toolkits.mplot3d import Axes3D
from collections import defaultdict


def pezw2(B, A):
    """
    pezw2(B,A)

    PEZW2(B,A) plots
    (a) the magnitude of H(z),
    (b) the zeros and poles (with the unit circle for reference),
    (c) the magnitude and phase spectrum of the system,
    (d) the impulse response h[n] (the first 100 samples), and
    (e) the group delay of the system.

    B and A are polyonomials in z^-1:
    B(z): vector containing the coefficients of numerator in descending
    order of z^-1
    A(z): vector containing the coefficients of denominator in descending
    order of z^-1

    Examples:
    for transfer function H(z) = 1/(1-0.4z^-1), call pezw2([1], [1, -0.4]);
    for transfer function H(z) = 1/(1 - 0.6exp(jπ/2)z^-1)(1 - 0.6exp(-jπ/2)z^-1), call
    pezw2([1], np.convolve([1, 0.6*np.exp(1j*np.pi/2)], [1, 0.6*np.exp(-1j*np.pi/2)])
    """
    fig = plt.figure(figsize=(16, 10))
    warnings.filterwarnings("ignore")
    ax1 = plt.subplot(2, 3, 2)
    # create the unit circle
    ax1 = fig.gca()
    uc = patches.Circle((0.0, 0.0), radius=1, fill=False,
                        color='black', ls='dashed', visible=True, alpha=0.1)
    ax1.add_patch(uc)
    ax1.add_artist(uc)

    x = np.linspace(-1.5, 1.5, 100)
    line = 0 * x
    ax1.plot(x, line, '--', color='black')
    ax1.plot(line, x, '--', color='black')

    # Get the poles and zeros
    if (len(A) > 1) & (len(B) > 1):
        z, p, k = sig.tf2zpk(B, A)

        # Plot the zeros and poles
        if len(z) > 0:
            ax1.plot(np.real(z), np.imag(z), 'go', markersize=10, label='Zeros')

        if len(p) > 0:
            ax1.plot(np.real(p), np.imag(p), 'rx', markersize=8, markeredgewidth=2, label='Poles')

        d = defaultdict(int)
        d1 = defaultdict(int)
        for i in p:
            d[i] += 1
        for item, count in d.items():
            if count > 1:
                plt.text(np.real(item), np.imag(item), r' ${}^{' + str(count) + '}$', fontsize=13)

        for i in z:
            d1[i] += 1
        for item, count in d1.items():
            if count > 1:
                plt.text(np.real(item), np.imag(item), r' ${}^{' + str(count) + '}$', fontsize=13)
    else:
        z, p, k = sig.tf2zpk(B, A)
        if len(A) == 1:
            ax1.plot(0.0, 0.0, 'rx', markersize=10, label='Poles')
            ax1.plot(np.real(z), np.imag(z), 'go', markersize=10, label='Zeros')

            d2 = defaultdict(int)
            count1 = 0
            for i in B:
                d2[i] += 1
            for item, count in d2.items():
                if count > 1:
                    plt.text(np.real(item), np.imag(item), r' ${}^{' + str(count) + '}$', fontsize=18)
                else:
                    count1+=1
                    if count1 == len(B):
                        plt.text(0.0, 0.0, r' ${}^{' + str(count1-1) + '}$', fontsize=18)

        elif len(B) == 1:
            ax1.plot(0.0, 0.0, 'ro', markersize=10, label='Zeros')
            ax1.plot(np.real(p), np.imag(p), 'rx', markersize=8, markeredgewidth=2, label='Poles')

            d3 = defaultdict(int)
            count2 = 0
            for i in A:
                d3[i] += 1
            for item, count in d3.items():
                if count > 1:
                    plt.text(np.real(item), np.imag(item), r' ${}^{' + str(count) + '}$', fontsize=18)
                else:
                    count2+=1
                    if count2 == len(A):
                        plt.text(0.0, 0.0, r' ${}^{' + str(count2-1) + '}$', fontsize=18)


    # Set axis limits
    # ax1.set_xlim(-1.5, 1.5)
    # ax1.set_ylim(-1.5, 1.5)

    # Set axis labels
    ax1.set_xlabel('Real Part')
    ax1.set_ylabel('Imaginary Part')

    w, H = sig.freqz(B, A, worN=256, whole=True)
    w = w - np.pi
    H = np.concatenate((H[128:256], H[0:128]))

    plt.subplot(2, 3, 3)
    plt.plot(w, (np.abs(H)))
    plt.grid(True)
    v = plt.axis()
    plt.axis([w[0], w[255], 0, 1.25*np.max(np.abs(H))])
    plt.xticks([-3, -2, -1, 0, 1, 0, 2, 3])
    plt.title('Magnitude')
    plt.xlabel('Frequency')

    plt.subplot(2, 3, 4)
    plt.plot(w, np.angle(H))
    plt.grid(True)
    v = plt.axis()
    #plt.axis([w[0], w[255], v[2], v[3]])
    plt.ylim(-4, 4)
    plt.xticks([-3, -2, -1, 0, 1, 2, 3])
    plt.title('Phase')
    plt.xlabel('Frequency')

    plt.subplot(2, 3, 5)
    # Define the impulse sequence of length 100
    impulse = np.repeat(0., 100)
    impulse[0] = 1.
    x = np.arange(0, 100)

    # Compute the impulse response
    h = sig.lfilter(B, A, impulse)
    plt.stem(x, h)
    plt.grid(True)
    plt.xlim(0, 100)
    plt.title('Impulse Resp.')
    plt.xlabel('Samples')

    plt.subplot(2, 3, 6)
    A1 = np.array(A)
    B = np.array(B)

    if len(B) > 1:
        B = B.flatten()

    w, h = sig.group_delay((B, A1), w=256, whole=True)
    #h = -np.diff(np.unwrap(np.angle(H)))/np.diff(w)
    w = w - np.pi
    h = np.concatenate((h[128:257], h[0:128]))
    plt.plot(w, h)
    plt.grid(True)
    v = plt.axis()
    plt.axis([w[0], w[255], v[2], v[3]])
    plt.xlim(-3, 3)
    plt.title('Group Delay')
    plt.xlabel('Frequency')

    ax = fig.add_subplot(231, projection='3d')

    num_pts = 46
    upplus = 2

    # Compute poles and zeros
    z, p, _ = sig.tf2zpk(B, A)

    mn = min([-1.2, *np.real(z), *np.real(p), *np.imag(z), *np.imag(p)])
    mx = max([1.2, *np.real(z), *np.real(p), *np.imag(z), *np.imag(p)])
    x, y = np.meshgrid(np.linspace(mn, mx, num=num_pts), np.linspace(mn, mx, num=num_pts))
    z = x + 1j * y

    ce = np.exp(-1j * 2 * np.pi * np.linspace(0, 1, 1000))
    cevals = np.abs(np.polyval(B, ce) / np.polyval(A, ce))
    vals = np.polyval(B[::-1], 1 / z) / np.polyval(A[::-1], 1 / z)
    vals[abs(vals) > np.max(cevals) + upplus] = np.max(cevals) + upplus

    ax.plot(np.real(ce), np.imag(ce), cevals + 0.01, color=[1, 0, 0], linewidth=2, alpha=1)
    ax.plot(np.real(ce), np.imag(ce), cevals - 0.01, color=[1, 0, 0], linewidth=2, alpha=1)

    ax.plot_surface(x, y, np.abs(vals), cmap='viridis', norm=LogNorm(), rstride=1, cstride=1, alpha=0.8)
    plt.axis('tight')

    ax.set_xlabel('Real (z)')
    ax.set_ylabel('Imag (z)')
    ax.set_zlabel('|H(z)|')
    plt.title('Magnitude of function H(z)')
    ax.dist = 12

    # Set the viewing angle (elevation, azimuth)
    ax.view_init(elev=20, azim=45)

    # Set the limits of the x and y axes
    ax.set_xlim(1, -1)
    ax.set_ylim(1, -1)

    plt.show()
    return
