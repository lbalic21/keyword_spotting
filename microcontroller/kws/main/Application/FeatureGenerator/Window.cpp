#include "Window.hpp"
#include "Configuration.hpp"
#include <math.h>
#include "esp_log.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Window::Window()
{
    ESP_LOGI("HANN", "Creating hann window");
    const float arg = M_PI * 2.0 / WINDOW_SIZE;

    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        float value = 0.5 * (1 - cos(arg * (i + 0.5)));
        this->data[i] = static_cast<int16_t>(value * (1 << WINDOW_BITS) + 0.5);
        //ESP_LOGI("HANN", "window[%d] = %d", i, this->data[i]);
    }
}