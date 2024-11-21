#include "MelSpectrogram.hpp"
#include <math.h>
#include <cstdio>

float MelSpectrogram::hzToMel(float hz)
{
    return MEL_HIGH_FREQUENCY_Q * log10f(1.0f + hz / MEL_BREAK_FREQUENCY_HERTZ);
}

float MelSpectrogram::melToHz(float mel)
{
    return MEL_BREAK_FREQUENCY_HERTZ * (powf(10.0f, mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}

void MelSpectrogram::generate(uint32_t* spectrogram, float* melSpectrogram) {

    // Convert lower and upper frequencies to mel scale (using integer approximation)
    float lowerMel = hzToMel(LOWER_BAND_LIMIT);
    float upperMel = hzToMel(UPPER_BAND_LIMIT);
    float melStep = (upperMel - lowerMel) / (NUMBER_OF_MEL_BINS + 1);

    // Calculate mel points
    //printf("Mel Points Calculation:\n");
    for (int i = 0; i < NUMBER_OF_MEL_BINS + 2; i++) {
        melPoints[i] = melToHz(lowerMel + i * melStep);
        //printf("Mel Point %d: %f\n", i, melPoints[i]);
    }

    // Apply triangular filters
    for (int m = 0; m < NUMBER_OF_MEL_BINS; m++) {
        melSpectrogram[m] = 0.0; 
        float left = melPoints[m];
        float center = melPoints[m + 1];
        float right = melPoints[m + 2];

        // Debugging output for mel bin
        //printf("Mel Bin %d: left=%f, center=%f, right=%f\n", m, left, center, right);

        // Apply filter to the spectrogram
        for (int k = 0; k < NUMBER_OF_SPECTROGRAM_BINS; k++) {
            // Convert index to frequency
            float frequency = (k * SAMPLE_RATE) / (2.0 * (NUMBER_OF_SPECTROGRAM_BINS - 1));
            
            // Debugging output for frequency
            //printf("Frequency for bin %d: %f\n", k, frequency);

            // Left side of triangle
            if (frequency >= left && frequency <= center) {
                melSpectrogram[m] += ((frequency - left) * (float)spectrogram[k]/32768 ) / (center - left);

                // Debugging output for accumulation
                //printf("Adding (left): melSpectrogram[%d] += %f (increment: %f)\n", m, melSpectrogram[m], (frequency - left) * (float)spectrogram[k]/32768 / (center - left));
            }
            // Right side of triangle
            else if (frequency > center && frequency <= right) {
                melSpectrogram[m] += ((right - frequency) * (float)spectrogram[k]/32768 ) / (right - center);

                // Debugging output for accumulation
                //printf("Adding (right): melSpectrogram[%d] += %f (increment: %f)\n", m, melSpectrogram[m], ((right - frequency) * (float)spectrogram[k]/32768) / (right - center));
            }
        }
    }
}