#include "FeatureGeneratorFloat.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator Float";

FeatureGeneratorFloat::FeatureGeneratorFloat(WindowFloat* window, FFTFloat* fft, MelSpectrogramFloat* melSpectrogram, DCT* dct)
{
    //ESP_LOGI(TAG, "Creating Feature Generator");
    this->window = window;
    this->fft = fft;
    this->melSpectrogram = melSpectrogram;
    this->dct = dct;
}

bool FeatureGeneratorFloat::generateFeatures(int16_t* audioFrame, float* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    float audioWindow[WINDOW_SIZE];
    window->apply(audioFrame, audioWindow);
    
    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    float spectrogram[NUMBER_OF_SPECTROGRAM_BINS];
    fft->compute(audioWindow, spectrogram);

    //for(int i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    ////{
    ////    printf("FFT[%d] = %f\n", i, spectrogram[i]);
    //}

    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/

    float melSpectro[NUMBER_OF_MEL_BINS] = {0.0};
    melSpectrogram->generate(spectrogram, melSpectro);

    //for(size_t i = 0; i < NUMBER_OF_MEL_BINS; i++)
    //{
    //    printf("MEL[%d] = %f\n", i, melSpectro[i]);
    //}

    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    float mfccs[13];
    dct->compute(melSpectro, mfccs);
    
    for(size_t i = 0; i < 13; i++)
    {
        //printf("MFCC[%d] = %f ", i, mfccs[i]);
        //printf("%f ", mfccs[i]);
        featureSlice[i] = mfccs[i];
    }
    printf("\n");

    return true;
}