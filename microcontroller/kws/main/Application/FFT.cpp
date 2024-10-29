#include "FFT.hpp"

FFT::FFT()
{
    dsps_fft2r_init_sc16(NULL, WINDOW_SIZE);
}

void FFT::compute(int16_t* frame, uint32_t* spectrogram)
{
    for(size_t i = 0; i < WINDOW_SIZE; i++)
    {
        data[2 * i] = frame[i];
        data[2 * i + 1] = 0;
    }

    dsps_fft2r_sc16_ae32(data, WINDOW_SIZE);
    dsps_bit_rev_sc16_ansi(data, WINDOW_SIZE);

    for(size_t i = 0; i < NUMBER_OF_SPECTROGRAM_BINS; i++)
    {
        spectrogram[i] = data[2 * i] * data[2 * i + 1];
    }
}