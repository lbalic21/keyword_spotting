#include "FeatureGenerator.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator";

bool FeatureGenerator::generateFeatures(int16_t* audioFrame, float* featureSlice)
{
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