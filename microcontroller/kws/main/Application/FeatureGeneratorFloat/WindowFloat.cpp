#include "WindowFloat.hpp"
#include "Configuration.hpp"
#include <math.h>
#include "esp_log.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

WindowFloat::WindowFloat()
{
    ESP_LOGI("HANN", "Creating hann window of floats");

    /*
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        this->data[i] = 0.54 - 0.46 * cos((2 * M_PI * i) / (WINDOW_SIZE - 1));
        //ESP_LOGI("HANN", "window[%d] = %d", i, this->data[i]);
    }
    */
    //dsps_wind_hann_f32(this->data, WINDOW_SIZE);
}