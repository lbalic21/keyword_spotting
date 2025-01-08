#include "MelSpectrogramFloat.hpp"
#include <math.h>
#include <cstdio>

MelSpectrogramFloat::MelSpectrogramFloat()
{
    this->lowerMel = 2595.0 * log10(1.0 + LOWER_BAND_LIMIT / 700.0);  // Lower frequency in Mel scale
    this->upperMel = 2595.0 * log10(1.0 + UPPER_BAND_LIMIT / 700.0);  // Upper frequency in Mel scale
    this->melStep = (this->upperMel - this->lowerMel) / (NUMBER_OF_MEL_BINS + 1); 
    this->hzPerBin = SAMPLE_RATE / WINDOW_SIZE;
    for (int i = 0; i < NUMBER_OF_MEL_BINS + 2; i++) {
        this->melPoints[i] = 700.0 * (pow(10.0, (this->lowerMel + this->melStep * i) / 2595.0) - 1.0);  // Mel to Hz
    }
}

void MelSpectrogramFloat::generate(float *spectrogram, float *melSpectrogram) {

    for (int melBin = 0; melBin < NUMBER_OF_MEL_BINS; melBin++) {
        melSpectrogram[melBin] = 0.0;
        for (int fftBin = 0; fftBin < NUMBER_OF_SPECTROGRAM_BINS; fftBin++) {
            float freq = fftBin * hzPerBin;
            float weight = 0.0;

            if (freq >= melPoints[melBin] && freq < melPoints[melBin + 1]) {
                weight = (freq - melPoints[melBin]) / (melPoints[melBin + 1] - melPoints[melBin]);
            } else if (freq >= melPoints[melBin + 1] && freq < melPoints[melBin + 2]) {
                weight = (melPoints[melBin + 2] - freq) / (melPoints[melBin + 2] - melPoints[melBin + 1]);
            }

            melSpectrogram[melBin] += spectrogram[fftBin] * weight;
        }
        // Apply log scaling with a safeguard against log(0)
        melSpectrogram[melBin] = logf(fmaxf(melSpectrogram[melBin], 1e-6));
    }
}

