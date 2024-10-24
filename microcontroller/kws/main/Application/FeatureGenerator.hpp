#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "stdbool.h"

class FeatureGenerator
{
    private:
        size_t inputAudioFrameSize;
        size_t outputFeatureSliceSize;

    public:
        FeatureGenerator(size_t inputAudioFrameSize, size_t outputFeatureSliceSize) : inputAudioFrameSize(inputAudioFrameSize),
                                                                                      outputFeatureSliceSize(outputFeatureSliceSize) {}

        bool generateFeatures(int16_t* audioFrame, int8_t* featureSlice);


};