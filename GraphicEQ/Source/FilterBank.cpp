#include "FilterBank.h"

Filterbank::Filterbank()
{
    for (int k = 0; k < order-1; k++)
        gainMultipliers[k] = 1.0f;
}

void Filterbank::setGain (float gainDB, int band)
{
    gainMultipliers[band] = Decibels::decibelsToGain (gainDB);
}

void Filterbank::reset()
{
    for (int n = 0; n < fftSize; n++)
        fftInput[n] = 0.0f;

    posCells.clear();
    negCells.clear();

    for (int i = 0; i < order-1; i++)
    {
        Array<Complex<float>> newPosCell;
        newPosCell.resize ((int) pow (2, i));
        posCells.add (newPosCell);

        Array<Complex<float>> newNegCell;
        newNegCell.resize ((int) pow (2, i));
        negCells.add (newNegCell);
    }

    buffers[0].setSize (1, fftSize);
    buffers[1].setSize (1, fftSize);

    buffers[0].clear();
    buffers[1].clear();
}

void Filterbank::process (float* buffer, int numSamples)
{
    // write to buffers
    if (writePtr + numSamples < windowSize)
    {
        buffers[writeBuffer].copyFrom (0, writePtr, buffer, numSamples);
        writePtr += numSamples;
    }
    else
    {
        auto samplesBeforeEnd = windowSize - writePtr;
        buffers[writeBuffer].copyFrom (0, writePtr, buffer, samplesBeforeEnd);

        doFFTProcessing (buffers[writeBuffer].getWritePointer (0), buffers[writeBuffer].getNumSamples());

        writeBuffer = ! writeBuffer;

        buffers[writeBuffer].copyFrom (0, 0, &buffer[samplesBeforeEnd], numSamples-samplesBeforeEnd);
        writePtr = numSamples-samplesBeforeEnd;
    }

    // read from buffers
    if (readPtr + numSamples < windowSize)
    {
        auto* bufferPtr = buffers[readBuffer].getReadPointer (0);
        auto* overlapBufferPtr = buffers[! readBuffer].getWritePointer(0);
        for (int n = 0; n < numSamples; n++)
        {
            buffer[n] = bufferPtr[readPtr+n];
            buffer[n] += overlapBufferPtr[readPtr + windowSize + n];
            overlapBufferPtr[readPtr + windowSize + n] = 0.0f;
        }

        readPtr = readPtr + numSamples;
    }
    else
    {
        auto samplesBeforeEnd = windowSize - readPtr;

        auto* bufferPtr = buffers[readBuffer].getReadPointer (0);
        auto* overlapBufferPtr = buffers[! readBuffer].getWritePointer(0);
        for (int n = 0; n < samplesBeforeEnd; n++)
        {
            buffer[n] = bufferPtr[readPtr+n];
            buffer[n] += overlapBufferPtr[readPtr + windowSize + n];
            overlapBufferPtr[readPtr + windowSize + n] = 0.0f;
        }

        readBuffer = ! readBuffer;

        bufferPtr = buffers[readBuffer].getReadPointer (0);
        overlapBufferPtr = buffers[! readBuffer].getWritePointer(0);
        for (int n = samplesBeforeEnd; n < numSamples; n++)
        {
            buffer[n] = bufferPtr[n-samplesBeforeEnd];
            buffer[n] += overlapBufferPtr[windowSize + n - samplesBeforeEnd];
            overlapBufferPtr[windowSize + n - samplesBeforeEnd] =  0.0f;
        }

        readPtr = numSamples-samplesBeforeEnd;
    }
}

void Filterbank::doFFTProcessing (float* buffer, int numSamples)
{
    for (int n = 0; n < numSamples; n++)
        fftInput[n] = buffer[n];

    fft.perform (fftInput, fftOutput, false);

    for (int k = 0; k < fftSize/2; k++)
    {
        positiveFreq[k] = fftOutput[k];
        negativeFreq[k] = fftOutput[fftSize-k-1];
    }

    // pack dyadic cells with FFT Data
    dcells (positiveFreq, posCells);
    dcells (negativeFreq, negCells);

    //multiply filter bands by gains
    multGains (posCells);
    multGains (negCells);

    // re-pack dyadic cells
    dcells2spec (positiveFreq, posCells);
    dcells2spec (negativeFreq, negCells);

    for (int k = 0; k < fftSize/2; k++)
    {
        iFftInput[k] = positiveFreq[k];
        iFftInput[fftSize-k-1] = negativeFreq[k];
    }

    fft.perform (iFftInput, iFftOutput, true);

    for (int n = 0; n < numSamples; n++)
        buffer[n] = iFftOutput[n].real();
}

void Filterbank::dcells (Complex<float>* fftData, Array<Array<Complex<float>>>& cells)
{
    int k = 0;
    for (auto& cell : cells)
    {
        for (auto& bin : cell)
        {
            bin = fftData[k];
            k++;
        }
    }
}

void Filterbank::dcells2spec (Complex<float>* fftData, Array<Array<Complex<float>>>& cells)
{
    int k = 0;
    for (auto& cell : cells)
    {
        for (auto& bin : cell)
        {
            fftData[k] = bin;
            k++;
        }
    }
}

void Filterbank::multGains (Array<Array<Complex<float>>>& cells)
{
    int k = 0;
    for (auto& cell : cells)
    {
        for (auto& bin : cell)
            bin *= gainMultipliers[k];
        k++;
    }
}
