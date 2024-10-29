#ifndef _FFT_H_
#define _FFT_H_

#include "esp_dsp.h"
#include "Configuration.hpp"

class FFT
{
    private:
        int16_t data[WINDOW_SIZE * 2];
        
    public:
        FFT();
        void compute(int16_t* frame, uint32_t* spectrogram);
};

#endif /* _FFT_H_*/