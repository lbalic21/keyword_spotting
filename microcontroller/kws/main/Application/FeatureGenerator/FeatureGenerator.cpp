#include "FeatureGenerator.hpp"
#include "dsps_fft2r.h"
#include "dsps_wind.h"
#include "math.h"

static const char *TAG = "Feature Generator";

template <typename T>
FeatureGenerator<T>::FeatureGenerator(Window* window, FFT* fft, MelSpectrogram* melSpectrogram)
{
    ESP_LOGI(TAG, "Creating Feature Generator");
    this->window = window;
    this->fft = fft;
    this->melSpectrogram = melSpectrogram;
}

template <typename T>
bool FeatureGenerator<T>::generateFeatures(int16_t* audioFrame, T* featureSlice)
{
    /*****************************************************************/
    /*************************** WINDOW ******************************/
    /*****************************************************************/

    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        int32_t value = audioFrame[i] * window->data[i];
        windowedSignal[i] = value >> WINDOW_BITS;
    }

    /*****************************************************************/
    /*************************** FFT *********************************/
    /*****************************************************************/

    fft->compute(windowedSignal, spectrogram);

    /*****************************************************************/
    /*********************** MEL SPECTRO *****************************/
    /*****************************************************************/

    this->melSpectrogram->generate(spectrogram, melEnergies);

    /*****************************************************************/
    /*************************** DCT *********************************/
    /*****************************************************************/

    for(size_t i = 0; i < NUMBER_OF_MFCCS; i++)
    {
        featureSlice[i] = static_cast<T>(melEnergies[i]); // Cast mel energies to T (int8_t or float)
    }

    return true;
}

// Explicit template instantiation
template class FeatureGenerator<int8_t>;
template class FeatureGenerator<float>;
