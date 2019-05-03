import librosa
import librosa.display
import matplotlib.pyplot as plt
import numpy as np

x, sr = librosa.load ('test.wav')

C = librosa.cqt(x, sr=sr)#, fmin=30, n_bins=16, bins_per_octave=2, hop_length=2**8)
# print (C[:,5])
# C[0:3,:] = 0
# C[4:8,:] = 0

librosa.display.specshow(librosa.amplitude_to_db(np.abs (C), ref=np.max),
                         sr=sr, x_axis='time', y_axis='cqt_note')
plt.colorbar(format='%+2.0f dB')
plt.title('Constant-Q power spectrum')
plt.tight_layout()


y = librosa.icqt (C, sr)#,  fmin=30, bins_per_octave=2, hop_length=2**8)
librosa.output.write_wav ('testOut.wav', y, sr, norm=True)

plt.show()
