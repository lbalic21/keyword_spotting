#include "Window.hpp"
#include "Configuration.hpp"
#include <math.h>
#include "esp_log.h"
#include "dsps_wind.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Window::Window()
{
    //ESP_LOGI("HANN", "Creating hann window of floats");

    
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        this->data[i] = 0.54 - 0.46 * cos((2 * M_PI * i) / (WINDOW_SIZE - 1));
        //ESP_LOGI("HANN", "window[%d] = %d", i, this->data[i]);
    }
    
    //dsps_wind_hann_f32(this->data, WINDOW_SIZE);
}

void Window::apply(int16_t* input, float* output)
{
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        output[i] = input[i] * this->data[i] / 32768;
        //printf("WINDOWED %d - %f\n", i, audioFloat[i]);
    }
}