#include "Application.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "AudioRecorder.hpp"

static const char *TAG = "MAIN";

void Application(void)
{
    AudioRecorder* audioRecorder = new AudioRecorder();

    while(1)
    {
        ESP_LOGI(TAG, "Running");
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}