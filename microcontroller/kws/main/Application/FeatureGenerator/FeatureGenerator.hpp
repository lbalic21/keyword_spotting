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
#include "DCT.hpp"

class FeatureGenerator
{
    public:
        bool generateFeatures(int16_t* audioFrame, float* featureSlice);

    private:
        Window window;
        FFT fft;
        MelSpectrogram melSpectrogram;
        DCT dct;
};

#endif /* FEATURE_GENERATOR_H */