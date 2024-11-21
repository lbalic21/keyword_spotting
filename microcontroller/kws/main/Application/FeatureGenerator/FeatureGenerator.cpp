#include "FeatureGenerator.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator";

FeatureGenerator::FeatureGenerator(Window* window, FFT* fft, MelSpectrogram* melSpectrogram)
{
    ESP_LOGI(TAG, "Creating Feature Generator");
    this->window = window;
    this->fft = fft;
    this->melSpectrogram = melSpectrogram;
}

bool FeatureGenerator::generateFeatures(int16_t* audioFrame, int8_t* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    int16_t maximumAbsoluteValue = 0;

    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        int32_t value = audioFrame[i] * window->data[i];
        //printf("%d - data %d * window %d = %ld\n", i, audioFrame[i], window->data[i], (value >> WINDOW_BITS));
        audioFrame[i] = value >> WINDOW_BITS;

        int16_t newAbsValue = (audioFrame[i] > 0) ? audioFrame[i] : -audioFrame[i];
        if(newAbsValue > maximumAbsoluteValue)
        {
            maximumAbsoluteValue = newAbsValue;
        }
    }

    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    // Apply the FFT to the window's output (and scale it so that the fixed point
    // FFT can have as much resolution as possible).

    //int shift = __builtin_clz(maximumAbsoluteValue);
    //printf("value %d shift %d\n", maximumAbsoluteValue, shift);

    uint32_t spectrogram[NUMBER_OF_SPECTROGRAM_BINS];
    fft->compute(audioFrame, spectrogram);

    
    for(int i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    {
        printf("%d -> %ld\n", i, spectrogram[i]);
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