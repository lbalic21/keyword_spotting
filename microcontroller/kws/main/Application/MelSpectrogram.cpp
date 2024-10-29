#include "MelSpectrogram.hpp"
#include <math.h>
#include <cstdio>

float MelSpectrogram::hzToMel(float hz)
{
    return MEL_HIGH_FREQUENCY_Q * log10f(1.0f + hz / MEL_BREAK_FREQUENCY_HERTZ) * FIXED_POINT_ONE;
}

float MelSpectrogram::melToHz(float mel)
{
    return MEL_BREAK_FREQUENCY_HERTZ * (powf(10.0f, mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}

// Fixed-point version of the Mel spectrogram generation
void MelSpectrogram::generate(uint32_t* spectrogram, uint32_t* melSpectrogram) {
    // Initialize the mel spectrogram to zero
    for (int m = 0; m < NUMBER_OF_MEL_BINS; m++) {
        melSpectrogram[m] = 0;
    }

    // Convert lower and upper frequencies to mel scale (fixed-point)
    uint32_t lowerMel = static_cast<uint32_t>(hzToMel(LOWER_BAND_LIMIT) * FIXED_POINT_ONE);
    uint32_t upperMel = static_cast<uint32_t>(hzToMel(UPPER_BAND_LIMIT) * FIXED_POINT_ONE);
    uint32_t melStep = (upperMel - lowerMel) / (NUMBER_OF_MEL_BINS + 1);

    // Calculate mel points in fixed-point
    printf("Mel Points Calculation:\n");
    for (int i = 0; i < NUMBER_OF_MEL_BINS + 2; i++) {
        melPoints[i] = static_cast<uint32_t>(melToHz(lowerMel + i * melStep) * FIXED_POINT_ONE);
        printf("Mel Point %d: %lu\n", i, melPoints[i]);
    }

    // Apply triangular filters
    for (int m = 0; m < NUMBER_OF_MEL_BINS; m++) {
        uint32_t left = melPoints[m];
        uint32_t center = melPoints[m + 1];
        uint32_t right = melPoints[m + 2];

        // Debugging output for mel bin
        printf("Mel Bin %d: left=%lu, center=%lu, right=%lu\n", m, left, center, right);

        // Apply filter to the spectrogram
        for (int k = 0; k < NUMBER_OF_SPECTROGRAM_BINS; k++) {
            // Convert index to frequency (fixed-point)
            uint32_t frequency = static_cast<uint32_t>(k * SAMPLE_RATE / (2.0 * (NUMBER_OF_SPECTROGRAM_BINS - 1)) * FIXED_POINT_ONE);
            
            // Debugging output for frequency
            printf("Frequency for bin %d: %lu\n", k, frequency);

            // Left side of triangle
            if (frequency >= left && frequency <= center) {
                uint32_t increment = ((frequency - left) * FIXED_POINT_ONE / (center - left)) * spectrogram[k] >> FIXED_POINT_FRACTIONAL_BITS;
                melSpectrogram[m] += increment;

                // Debugging output for accumulation
                printf("Adding (left): melSpectrogram[%d] += %lu (increment: %lu)\n", m, melSpectrogram[m], increment);
            }
            // Right side of triangle
            else if (frequency > center && frequency <= right) {
                uint32_t increment = ((right - frequency) * FIXED_POINT_ONE / (right - center)) * spectrogram[k] >> FIXED_POINT_FRACTIONAL_BITS;
                melSpectrogram[m] += increment;

                // Debugging output for accumulation
                printf("Adding (right): melSpectrogram[%d] += %lu (increment: %lu)\n", m, melSpectrogram[m], increment);
            }
        }
    }
}
