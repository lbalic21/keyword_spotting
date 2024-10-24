#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"

#include "Application.hpp"
#include "AudioRecorder.hpp"
#include "FeatureGenerator.hpp"

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS(xTimeInMs) ((xTimeInMs * configTICK_RATE_HZ) / 1000)
#endif

static const char *TAG = "MAIN";

AudioRecorder audioRecorder(SAMPLE_RATE);
FeatureGenerator featureGenerator(WINDOW_SIZE, NUMBER_OF_MFCCS);

void Application(void)
{
    ESP_LOGI(TAG, "Application started");
    ESP_LOGI(TAG, "Time slices in a second: %d", NUMBER_OF_TIME_SLICES);
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "Stack high watermark: %d", stackHighWaterMark);

    audioRecorder.set();
    audioRecorder.start();
    
    int16_t audioFrame[WINDOW_SIZE] = {0}; 
    int8_t featureImage[NUMBER_OF_TIME_SLICES * NUMBER_OF_MFCCS] = {0};

    ESP_LOGI(TAG, "Starting the main system loop");
    TickType_t lastInferenceTicks = xTaskGetTickCount();
    TickType_t minimalInferenceTicks = pdMS_TO_TICKS(1000 / MAX_INFERENCES_PER_SECOND);

    while(1)
    {
        uint32_t bytesRead = audioRecorder.getSamples(audioFrame, WINDOW_SIZE);
        ESP_LOGI(TAG, "Samples retrieved: %ld (%ld bytes)", bytesRead / 2, bytesRead);

        bool success = featureGenerator.generateFeatures(audioFrame, featureImage);
        if(!success)
        {
            ESP_LOGE(TAG, "Generating feature unsuccessfull");
        }


        if(xTaskDelayUntil(&lastInferenceTicks, minimalInferenceTicks) != pdTRUE)
        {
            ESP_LOGE(TAG, "Sleep not working");
        }
    }
    
}