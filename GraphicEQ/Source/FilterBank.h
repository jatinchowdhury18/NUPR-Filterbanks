#ifndef FILTERBANK_H_INCLUDED
#define FILTERBANK_H_INCLUDED

#include "JuceHeader.h"
#include <vector>
#include <complex>

namespace FilterbankConstants
{
    enum
    {
        order = 12,
    };
    constexpr int fftSize = 4096; //pow (2, (int) order);
}

using namespace FilterbankConstants;
template <typename Type>
using Complex = std::complex<Type>;

class Filterbank
{
public:
    Filterbank();

    void reset();
    void process (float* buffer, int numSamples);

    void setGain (float gainDB, int band);

private:
    void dcells (Complex<float>* fftData, Array<Array<Complex<float>>>& cells);
    void dcells2spec (Complex<float>* fftData, Array<Array<Complex<float>>>& cells);
    void multGains (Array<Array<Complex<float>>>& cells);

    Complex<float> fftInput[fftSize];
    Complex<float> fftOutput[fftSize];

    Complex<float> positiveFreq[fftSize/2];
    Complex<float> negativeFreq[fftSize/2];

    Array<Array<Complex<float>>> posCells;
    Array<Array<Complex<float>>> negCells;

    Complex<float> iFftInput[fftSize];
    Complex<float> iFftOutput[fftSize];

    dsp::FFT fft = dsp::FFT (order);

    float gainMultipliers[order-1];
};

#endif //FILTERBANK_H_INCLUDED
