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
}

void Filterbank::process (float* buffer, int numSamples)
{
    for (int n = numSamples; n < fftSize; n++)
        fftInput[n] = fftInput[n-numSamples];

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
