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
    
    

    while(1)
    {
        ESP_LOGI(TAG, "Running");
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
    
}