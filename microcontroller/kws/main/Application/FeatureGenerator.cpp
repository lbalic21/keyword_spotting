#include "FeatureGenerator.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator";

FeatureGenerator::FeatureGenerator(Window* window)
{
    ESP_LOGI(TAG, "Creating Feature Generator");
    this->window = window;
}

bool FeatureGenerator::generateFeatures(int16_t* audioFrame, int8_t* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/


    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        int32_t value = audioFrame[i] * window->data[i];
        printf("%d - data %d * window %d = %ld\n", i, audioFrame[i], window->data[i], (value >> WINDOW_BITS));
        audioFrame[i] = value >> WINDOW_BITS;
    }

    

    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    // Initialize the ESP-DSP library
    //dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);


    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/


    /*****************************************************************/
    /********************* LOG MEL SPECTRO ***************************/
    /*****************************************************************/


    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    return true;
}