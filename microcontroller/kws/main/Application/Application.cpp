#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"

#include "Application.hpp"
#include "AudioRecorder.hpp"

static const char *TAG = "MAIN";

#define SAMPLE_RATE     16000

void Application(void)
{
    ESP_LOGI(TAG, "Application started");

    AudioRecorder* audioRecorder = new AudioRecorder(SAMPLE_RATE);
    audioRecorder->set();
    audioRecorder->start();
    
    int16_t audioFrame[480]; //30ms

    ESP_LOGI(TAG, "Starting the main system loop");
    TickType_t lastInferenceTicks = xTaskGetTickCount();

    while(1)
    {
        ESP_LOGI(TAG, "Running");

        uint32_t bytesRead = audioRecorder->getSamples(audioFrame, 480);
        ESP_LOGI(TAG, "Samples retrieved: %ld", bytesRead);


        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
    
}