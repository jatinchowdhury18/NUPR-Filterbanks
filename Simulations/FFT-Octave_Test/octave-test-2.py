import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sig
import scipy.io.wavfile as wav

def plot_fft (X, N, fs=1):
    f = fs * (np.arange (N) - N/2) / N
    plt.semilogx (f[int(N/2):int(N)], 20 * np.log10 (abs (np.fft.fftshift(X[0:int (N)])[int(N/2):int(N)])))
    # plt.semilogx (f[int(N/2):int(N)], np.unwrap (np.angle (np.fft.fftshift(X[0:int (N)])[int(N/2):int(N)])))
    # plt.axvline (x=16000, color='green')
    # plt.axvline (x=8000, color='green')
    # plt.axvline (x=4000, color='green')
    # plt.axvline (x=2000, color='green')
    # plt.axvline (x=1000, color='green')
    # plt.axvline (x=500, color='green')
    # plt.axvline (x=250, color='green')
    # plt.axvline (x=120, color='green')
    # plt.axvline (x=60, color='green')
    # plt.axvline (x=30, color='green')
    plt.ylim (ymin=-60)
    plt.show()

def dcells (X, lowbins=1):
    N = len(X)
    cells = []

    while N > lowbins:
        cells.append (X[int(N/2)-1:int(N-1)])
        N = N/2
    cells.append (np.array(X[0:lowbins-1]))
    cells.reverse()

    return cells


def dcells2spec (cells,N):
    # N = 2**len(cells)
    X = np.zeros (int(N), dtype=np.complex128)

    k = 0
    for cell in cells:
        if (np.size(cell) == 1):
            X[k] = cell
            k = k+1
            continue

        for bin in cell:
            X[k] = bin
            k = k+1

    return X


def filt (x, N):
    LN = round(np.log2(N)) # number of octave bands
    shifter = 1.0 #np.exp(0-1j * np.pi * np.arange(N)/N) # half-bin

    xs = x * shifter # apply spectral shift
    X = np.fft.fft (xs,N) # project xs onto rotated basis

    XP = X[0:int(N/2)] # positive-frequency components
    XN = X[N-1:int(N/2)-1:-1] # neg.-frequency components

    YP = dcells(XP) # partition to octave bands
    YN = dcells(XN) # ditto for neg. frequencies

    print (len (YP[1]))

    # YP[5] = 10 * YP[5]
    # YN[5] = 10 * YN[5]

    # YP[7] = 2.0 * YP[7]
    # YN[7] = 2.0 * YN[7]

    # YP[8] = 2.4 * YP[8]
    # YN[8] = 2.4 * YN[8]

    # YP[9] = 2 * YP[9]
    # YN[9] = 2 * YN[9]

    # YP[10] = 0.5 * YP[10]
    # YN[10] = 0.5 * YN[10]

    # YP[11] = 1.8 * YP[11]
    # YN[11] = 1.8 * YN[11]

    # YP[12] = 1.6 * YP[12]
    # YN[12] = 1.6 * YN[12]

    # YP[13] = 1.4 * YP[13]
    # YN[13] = 1.4 * YN[13]

    # YP[14] = 1.2 * YP[14]
    # YN[14] = 1.2 * YN[14]

    # YP[15] = 0.5 * YP[15]
    # YN[int(LN-1)] = 0.5 * YN[int(LN-1)]

    YPe = dcells2spec(YP,N/2) # unpack "dyadic cells"
    YNe = dcells2spec(YN,N/2) # unpack neg. freqs
    YNeflr = YNe[int(N/2)-1:0:-1] # undo former flip

    ys = np.fft.ifft (np.concatenate ((YPe,YNeflr)), N)
    y = np.real (np.conj(shifter) * ys) # = octave filter-bank signals (real)

    return y


M = 12 # number of octave bands
N = 2**M
hop = N/4

# impulse input
# x = np.zeros (N+1)
# x[int(N/2)] = 1

# Sine input
f = 400
fs = 44100
x_len = 2**17
x = np.sin (2 * np.pi * f * np.arange (x_len) / fs)

# Audio input
# fs, x = wav.read ('./test.wav')

window = sig.blackman (N, sym=False)

n = 0
y = np.zeros (len (x))
while (n + N < len(x)):
    x_part = x[int(n):int(n+N)]
    y[int(n):int(n+N)] = y[int(n):int(n+N)] + filt (window * x_part, N)
    n = n+hop

# plt.figure()
# plt.plot (y)

Y = np.fft.fft (y, N)
# Y = Y/max (abs (Y))
plt.figure()
plot_fft (Y, N, fs=44100)

# y = y / max (abs(y))
# wav.write ('test-out.wav', fs, y)

###
# M = 12 -> flat down to 20 Hz
# Band 1: 0Hz
# Band 2:  30
# Band 3:  60
# Band 4:  120
# Band 5:  250
# Band 6: 500
# Band 7: 1k
# Band 8: 2k
# Band 9: 4k
# Band 10: 8k
# Band 11: 16k
