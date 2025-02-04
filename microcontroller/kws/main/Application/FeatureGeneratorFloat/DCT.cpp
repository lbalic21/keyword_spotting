#include "DCT.hpp"

DCT::DCT()
{
    const float pi = 3.14159265358979323846f;
    for (int i = 0; i < NUMBER_OF_MFCCS+1; ++i) {
        for (int j = 0; j < NUMBER_OF_MEL_BINS; ++j) {
            coefficients[i * NUMBER_OF_MEL_BINS + j] = cos(pi * i * (j + 0.5f) / NUMBER_OF_MEL_BINS);
        }
    }
}

void DCT::compute(const float *input, float *output) {
    float temp[NUMBER_OF_MFCCS+1];
    for (int i = 0; i < NUMBER_OF_MFCCS + 1; i++) {
        temp[i] = 0.0;
        for (int j = 0; j < NUMBER_OF_MEL_BINS; j++) {
            temp[i] += input[j] * coefficients[i * NUMBER_OF_MEL_BINS + j];
        }
        temp[i] *= sqrt(2.0 / NUMBER_OF_MEL_BINS);
    }

    for (size_t i = 0; i < 12; i++)
    {
        output[i] = temp[i+1];
    }
    
}

