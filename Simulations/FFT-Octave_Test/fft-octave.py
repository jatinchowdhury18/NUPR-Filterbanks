import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sig
import scipy.io.wavfile as wav

N = 100

fs, x = wav.read ('./test.wav')

print (np.size (x))

X = np.fft.fft (x[:], N)

BandK = X[10:20]
z1 = np.zeros (10 - 1)
z2 = np.zeros (N - 20)

BandKzp= np.concatenate ((z1, BandK, z2))

x_k = np.fft.ifft (BandKzp, N)

X_k = X = np.fft.fft (x_k, N)

plt.plot (20 * np.log10 (abs (X_k[0:int (N/2)])))
plt.show()

