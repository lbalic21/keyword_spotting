#include "FeatureGenerator.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator";

#define ALPHA_Q15 31785  // 0.97 * (1 << 15) in Q15 format
void preemphasis_q15(int16_t *input, int16_t *output, size_t length) {
    if (length == 0) return;

    output[0] = input[0];

    for (size_t i = 1; i < length; i++) {
        output[i] = input[i] - ((ALPHA_Q15 * input[i - 1]) >> 15);
    }
}

void preemphasis(int16_t *input, int16_t *output, size_t length, float alpha = 0.97f) {
    if (length == 0) return;

    // First sample remains the same
    output[0] = input[0];

    // Apply pre-emphasis to remaining samples
    for (size_t i = 1; i < length; i++) {
        output[i] = input[i] - (int16_t)(alpha * input[i - 1]);
    }
}

bool FeatureGenerator::generateFeatures(int16_t* audioFrame, float* featureSlice)
{
    //preemphasize
    int16_t q[WINDOW_SIZE];
    preemphasis(audioFrame, q, WINDOW_SIZE);


    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    float audioWindow[WINDOW_SIZE];
    window.apply(audioFrame, audioWindow);
    
    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    float spectrogram[NUMBER_OF_SPECTROGRAM_BINS];
    fft.compute(audioWindow, spectrogram);

    //for(int i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    ////{
    ////    printf("FFT[%d] = %f\n", i, spectrogram[i]);
    //}

    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/

    float melSpectro[NUMBER_OF_MEL_BINS] = {0.0};
    melSpectrogram.generate(spectrogram, melSpectro);

    //for(size_t i = 0; i < NUMBER_OF_MEL_BINS; i++)
    //{
    //    printf("MEL[%d] = %f\n", i, melSpectro[i]);
    //}

    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    dct.compute(melSpectro, featureSlice);

    return true;
}