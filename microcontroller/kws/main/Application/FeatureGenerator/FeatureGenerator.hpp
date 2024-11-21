#ifndef FEATURE_GENERATOR_H
#define FEATURE_GENERATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "stdbool.h"
#include "Window.hpp"
#include "FFT.hpp"
#include "MelSpectrogram.hpp"

class FeatureGenerator
{
    public:
        FeatureGenerator(Window* window, FFT* fft, MelSpectrogram* melSpectrogram);
        bool generateFeatures(int16_t* audioFrame, int8_t* featureSlice);

    private:
        Window* window;
        FFT* fft;
        MelSpectrogram* melSpectrogram;
};

#endif