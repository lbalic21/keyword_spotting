#include "FFT.hpp"

FFT::FFT()
{
    dsps_fft2r_init_sc16(NULL, WINDOW_SIZE);
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

        spectrogram[i] = real * real + imag * imag;
    }
}