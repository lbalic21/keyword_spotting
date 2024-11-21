#ifndef _FFT_FLOAT_H_
#define _FFT_FLOAT_H_

#include "esp_dsp.h"
#include "Configuration.hpp"

class FFTFloat
{
    private:
        float data[WINDOW_SIZE * 2];
        
    public:
        FFTFloat();
        void compute(float* frame, float* spectrogram);
};

#endif /* _FFT_H_*/