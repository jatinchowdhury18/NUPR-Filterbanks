import numpy as np
import scipy.signal as signal
from scipy.io import wavfile
import matplotlib.pyplot as plt
import random

def zeropadzerophasewin(input_sig, window, padded_length):
    """
    function windows a signal and zero pads with zero phase
    """
    assert len(input_sig) == len(window)

    M = len(input_sig)

    output_sig = np.zeros ([padded_length])

    for n in range(len(input_sig)):
        index = (n - int(M/2)) % padded_length
        output_sig[index] = input_sig[n] * window[n]
    return output_sig

class Filterbank:
    """  """
    def __init__(self):

        self.numBuffers = 3
        self.buffers = []
        self.readBuffer = 0
        # self.readPtr = 0
        self.writeBuffer = 2
        self.writePtr = 0

        self.order = 12
        self.fftSize = 4096
        self.windowSize = int (self.fftSize / 2)

        self.fftInput = np.zeros (self.fftSize, np.complex128)
        self.fftOutput = np.zeros (self.fftSize, np.complex128)

        self.positiveFreq = np.zeros (int (self.fftSize / 2), np.complex128)
        self.negativeFreq = np.zeros (int (self.fftSize / 2), np.complex128)

        self.posCells = []
        self.negCells = []

        self.iFftInput = np.zeros (self.fftSize, np.complex128)
        self.iFftOutput = np.zeros (self.fftSize, np.complex128)

        self.gainMultipliers = np.ones (self.order-1)

    def reset (self, sampleRate, maxBlockSize):
        self.posCells.clear()
        self.negCells.clear()

        for i in range (self.order-1):
            newPosCell = np.zeros (2**i, dtype=np.complex128)
            self.posCells.append (newPosCell)

            newNegCell = np.zeros (2**i, dtype=np.complex128)
            self.negCells.append (newNegCell)

        self.buffers.clear()
        for _ in range (self.numBuffers):
            self.buffers.append (np.zeros (self.fftSize))

    def process (self, buffer, numSamples):
        doFFT = -1

        if (self.writePtr + numSamples < self.windowSize):
            for n in range (numSamples):
                self.buffers[self.writeBuffer][self.writePtr+n] = buffer[n]

            overlapBuffer = (self.readBuffer + 1) % self.numBuffers
            for n in range (numSamples):
                buffer[n] = self.buffers[self.readBuffer][self.writePtr+n]
                buffer[n] += self.buffers[overlapBuffer][self.writePtr+self.windowSize+n]
                self.buffers[overlapBuffer][self.writePtr+self.windowSize+n] = 0

            self.writePtr += numSamples
        else:
            samplesBeforeEnd = int (self.windowSize - self.writePtr)
            for n in range (samplesBeforeEnd):
                self.buffers[self.writeBuffer][self.writePtr+n] = buffer[n]

            overlapBuffer = (self.readBuffer + 1) % self.numBuffers
            for n in range (samplesBeforeEnd):
                buffer[n] = self.buffers[self.readBuffer][self.writePtr+n]
                buffer[n] += self.buffers[overlapBuffer][self.writePtr+self.windowSize+n]
                self.buffers[overlapBuffer][self.writePtr+self.windowSize+n] = 0

            doFFT = self.writeBuffer

            self.writeBuffer = (self.writeBuffer + 1) % self.numBuffers
            self.readBuffer = (self.readBuffer + 1) % self.numBuffers

            for n in range (numSamples-samplesBeforeEnd):
                self.buffers[self.writeBuffer][n] = buffer[samplesBeforeEnd+n]

            overlapBuffer = (self.readBuffer + 1) % self.numBuffers
            for n in range (numSamples-samplesBeforeEnd):
                buffer[samplesBeforeEnd+n] = self.buffers[self.readBuffer][n]
                buffer[samplesBeforeEnd+n] += self.buffers[overlapBuffer][self.windowSize+n]
                self.buffers[overlapBuffer][self.windowSize+n] = 0

            self.writePtr = numSamples-samplesBeforeEnd
        
        if (doFFT >= 0):
            self.doFFTProcessing (self.buffers[doFFT], self.fftSize)
            # plt.figure()
            # plt.plot (self.buffers[doFFT])

    def doFFTProcessing (self, buffer, numSamples):
        for n in range (numSamples):
            self.fftInput[n] = buffer[n]
        # for n in range (self.windowSize):
            # index = (n - int(self.windowSize/2)) % self.fftSize
            # self.fftInput[index] = buffer[n]
        # plt.plot (np.abs (self.fftInput))
        
        self.fftOutput = np.fft.fft (self.fftInput)

        for k in range (int (self.fftSize / 2)):
            self.positiveFreq[k] = self.fftOutput[k]
            self.negativeFreq[k] = self.fftOutput[int (self.fftSize-k-1)]

        # self.positiveFreq[100] *= 100
        # self.negativeFreq[100] *= 100
        # self.positiveFreq[101] *= 100
        # self.negativeFreq[101] *= 100

        self.dcells (self.positiveFreq, self.posCells)
        self.dcells (self.negativeFreq, self.negCells)

        # Process HERE
        mult = 4
        band = 5
        # for n in range (len (self.posCells)):
        #     # if (n < 1 or n > len (self.posCells)-2):
        #     #     self.posCells[band][n] = self.posCells[band][n] * mult / 10
        #     #     self.negCells[band][n] = self.negCells[band][n] * mult / 10
        #     # else: 
        #     self.posCells[band][n] = self.posCells[band][n] * mult
        #     self.negCells[band][n] = self.negCells[band][n] * mult

        self.posCells[band] = self.posCells[band] * 3
        self.negCells[band] = self.negCells[band] * 3
        # self.posCells[band+1] = self.posCells[band+1] * 10
        # self.negCells[band+1] = self.negCells[band+1] * 10
        # print (self.posCells[band])

        self.dcells2spec (self.positiveFreq, self.posCells)
        self.dcells2spec (self.negativeFreq, self.negCells)

        for k in range (int (self.fftSize / 2)):
            self.iFftInput[k] = self.positiveFreq[k]
            self.iFftInput[int (self.fftSize-k-1)] = self.negativeFreq[k]
        
        self.iFftOutput = np.fft.ifft (self.iFftInput)
        
        for n in range (numSamples):
            buffer[n] = np.real (self.iFftOutput[n])

    def dcells (self, fftData, cells):
        k = 0
        for n in range (len (cells)):
            for m in range (len (cells[n])):
                cells[n][m] = fftData[k]
                k += 1
    
    def dcells2spec (self, fftData, cells):
        k = 0
        for cell in cells:
            for bin in cell:
                fftData[k] = bin
                k += 1
    
    def multGains (self, cells):
        pass
    
def main():
    bufferSize = 256
    fs = 44100.0

    x = np.zeros (int (4096))
    x[10] = 1 #; x[400] = 1
    # fs, x = wavfile.read ('test.wav')
    # x = 0.5*x[:int (10*4096)] / np.max (np.abs (x))

    eq = Filterbank()
    eq.reset (fs, bufferSize)

    y = np.zeros (int (len(x) + 4096))

    n = 0
    while (n < len (y)):
        buff = random.choice ([bufferSize, int (bufferSize / 2)])
        frame = np.zeros (buff)

        if (n + buff < len(x)):
            frame[:] = x[n:n+buff]

        eq.process (frame, buff)

        # plt.plot (frame)

        if (n + buff < len(y)):
            y[n:n+buff] = frame[:]
        
        n += buff

    # wavfile.write ('test-out.wav', fs, (2**15 * y).astype(np.int16))

    Y = np.fft.rfft (y)

    plt.figure()
    plt.semilogx (np.linspace (0, fs/2, len (Y)), np.abs (Y))

    plt.figure()
    plt.plot (y)

main()
plt.show()
