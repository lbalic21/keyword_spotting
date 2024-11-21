#include "FeatureGeneratorFloat.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator Float";

FeatureGeneratorFloat::FeatureGeneratorFloat(WindowFloat* window, FFTFloat* fft, MelSpectrogramFloat* melSpectrogram)
{
    ESP_LOGI(TAG, "Creating Feature Generator");
    this->window = window;
    this->fft = fft;
    this->melSpectrogram = melSpectrogram;
}

bool FeatureGeneratorFloat::generateFeatures(int16_t* audioFrame, float* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    float windowedFrame[WINDOW_SIZE];

    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        windowedFrame[i] = audioFrame[i] * window->data[i] / 32768;
        //printf("%d - %f\n", i, windowedFrame[i]);
    }

    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    float spectrogram[NUMBER_OF_SPECTROGRAM_BINS];
    fft->compute(windowedFrame, spectrogram);

    
    for(int i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    {
        printf("FFT %d -> %f\n", i, spectrogram[i]);
    }
    

    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/

    float melSpectro[NUMBER_OF_MEL_BINS] = {0.0};
    this->melSpectrogram->generate(spectrogram, melSpectro);

    for(size_t i = 0; i < NUMBER_OF_MEL_BINS; i++)
    {
        printf("%d -> %f\n", i, melSpectro[i]);
    }
    
    /*****************************************************************/
    /********************* LOG MEL SPECTRO ***************************/
    /*****************************************************************/


    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    return true;
}