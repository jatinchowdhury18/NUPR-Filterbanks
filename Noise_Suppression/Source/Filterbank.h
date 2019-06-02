#ifndef FILTERBANK_H_INCLUDED
#define FILTERBANK_H_INCLUDED

#include "JuceHeader.h"
#include <vector>
#include <complex>

namespace FilterbankConstants
{
    enum
    {
        order44_1 = 12,
    };
    constexpr int fftSize44_1 = 4096; //pow (2, (int) order);
}

using namespace FilterbankConstants;
template <typename Type>
using Complex = std::complex<Type>;

class Filterbank
{
public:
    Filterbank();

    void reset (double sampleRate, int maxExpectedBlockSize);
    void process (float* buffer, int numSamples);

    void setThresh (float threshDB);
    void setSpeed (float speedMs);

    float getCellPower (int cellIndex) const { return cellPowers[cellIndex]; }

private:
    void doFFTProcessing (float* buffer, int numSamples);
    void dcells (Complex<float>* fftData, Array<Array<Complex<float>>>& cells);
    void dcells2spec (Complex<float>* fftData, Array<Array<Complex<float>>>& cells);
    void calcCellPower (Array<Array<Complex<float>>>& pCells, Array<Array<Complex<float>>>& nCells, float* powers);
    void setGains (float* powers);
    void multGains (Array<Array<Complex<float>>>& cells);

    AudioBuffer<float> buffers[2];
    int readBuffer = 1;
    int readPtr = 0;
    int writeBuffer = 0;
    int writePtr = 0;

    int order = order44_1;
    int fftSize = fftSize44_1;
    int windowSize = fftSize / 2;

    Complex<float> fftInput[4*fftSize44_1];
    Complex<float> fftOutput[4*fftSize44_1];

    Complex<float> positiveFreq[4*fftSize44_1 / 2];
    Complex<float> negativeFreq[4*fftSize44_1 / 2];

    Array<Array<Complex<float>>> posCells;
    Array<Array<Complex<float>>> negCells;
    float cellPowers[order44_1-1];

    Complex<float> iFftInput[4*fftSize44_1];
    Complex<float> iFftOutput[4*fftSize44_1];

    std::unique_ptr<dsp::FFT> fft;

    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> gainMultipliers[order44_1-1];

    float thresh = 0.0f;
    float speedSeconds = 0.0f;
    float fs = 44100.0f;
};

#endif //FILTERBANK_H_INCLUDED
