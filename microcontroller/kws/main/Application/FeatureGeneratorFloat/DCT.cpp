#include "DCT.hpp"

DCT::DCT()
{
    const float pi = 3.14159265358979323846f;
    for (int i = 0; i < NUMBER_OF_MFCCS; ++i) {
        for (int j = 0; j < NUMBER_OF_MEL_BINS; ++j) {
            coefficients[i * NUMBER_OF_MEL_BINS + j] = cos(pi * i * (j + 0.5f) / NUMBER_OF_MEL_BINS);
        }
    }
}

void DCT::compute(const float *input, float *output) {
    for (int i = 0; i < NUMBER_OF_MFCCS; i++) {
        output[i] = 0.0;
        for (int j = 0; j < NUMBER_OF_MEL_BINS; j++) {
            output[i] += input[j] * coefficients[i * NUMBER_OF_MEL_BINS + j];
        }
        output[i] *= sqrt(2.0 / NUMBER_OF_MEL_BINS);
    }
}

