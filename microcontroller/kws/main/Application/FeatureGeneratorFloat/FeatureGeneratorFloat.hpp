#ifndef FEATURE_GENERATOR_FLOAT_H
#define FEATURE_GENERATOR_FLOAT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "stdbool.h"
#include "WindowFloat.hpp"
#include "FFTFloat.hpp"
#include "MelSpectrogramFloat.hpp"

class FeatureGeneratorFloat
{
    public:
        FeatureGeneratorFloat(WindowFloat* window, FFTFloat* fft, MelSpectrogramFloat* melSpectrogram);
        bool generateFeatures(int16_t* audioFrame, float* featureSlice);

    private:
        WindowFloat* window;
        FFTFloat* fft;
        MelSpectrogramFloat* melSpectrogram;
};

#endif