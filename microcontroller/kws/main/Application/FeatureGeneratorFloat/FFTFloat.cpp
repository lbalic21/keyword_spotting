#include "FFTFloat.hpp"
#include <math.h>

FFTFloat::FFTFloat()
{
    esp_err_t ret = dsps_fft2r_init_fc32(NULL, WINDOW_SIZE);
    if(ret != ESP_OK)
    {
        ESP_LOGE("FFT", "Not possible to initialize FFT. Error = %i", ret);
        while(1);
    }
}

void FFTFloat::compute(float* frame, float* spectrogram)
{
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        data[2 * i] = frame[i];  // Real part
        data[2 * i + 1] = 0;     // Imaginary part
    }

    // Perform FFT
    dsps_fft2r_fc32_ae32(data, WINDOW_SIZE);

    // Bit reversal
    dsps_bit_rev_fc32_ansi(data, WINDOW_SIZE);

    // Magnitude (power) of each frequency bin
    for(size_t i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    {
        float real = data[2 * i];
        float imag = data[2 * i + 1];

        spectrogram[i] = sqrt(real * real + imag * imag);
    }
}