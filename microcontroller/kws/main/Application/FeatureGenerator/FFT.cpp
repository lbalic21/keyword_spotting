#include "FFT.hpp"
#include "kiss_fft.h"

FFT::FFT()
{
    if(dsps_fft2r_init_sc16(NULL, WINDOW_SIZE) != ESP_OK)
    {
        //ESP_LOGE("FFT", "FFT could not initialize!");
        while(1);
    }
}

void FFT::compute(int16_t* frame, uint32_t* spectrogram)
{
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        data[2 * i] = frame[i];  // Real part
        data[2 * i + 1] = 0;     // Imaginary part
    }

    // Perform FFT
    dsps_fft2r_sc16_ae32(data, WINDOW_SIZE);

    // Bit reversal
    dsps_bit_rev_sc16_ansi(data, WINDOW_SIZE);

    // Magnitude (power) of each frequency bin
    for(size_t i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    {
        int32_t real = data[2 * i];
        int32_t imag = data[2 * i + 1];

        int64_t real_squared = (int64_t)real * real;
        int64_t imag_squared = (int64_t)imag * imag;

        spectrogram[i] = (uint32_t)(real_squared + imag_squared);
    }
}