#ifndef FEATURE_GENERATOR_H
#define FEATURE_GENERATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "stdbool.h"
#include "Window.hpp"
#include "FFT.hpp"
#include "MelFilterbank.hpp"

template <typename T, typename K>
class FeatureGenerator
{
    public:
        FeatureGenerator();
        bool generateFeatures(int16_t* audioFrame, int8_t* featureSlice);

    private:

        Window<T> window;
        FFT<T> fft;
        Melfiltebank<T> melFilterbank;
        DCT<K> dct;

        T windowedSignal[WINDOW_SIZE];
        T spectrogram[NUMBER_OF_SPECTROGRAM_BINS];
        T melSpectrogram[NUMBER_OF_MEL_BINS];
};



#endif
