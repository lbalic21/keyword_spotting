#ifndef _FFT_H_
#define _FFT_H_

#include "esp_dsp.h"
#include "Configuration.hpp"

class FFT
{
    private:
        float data[WINDOW_SIZE * 2];
        
    public:
        FFT();
        void compute(float* frame, float* spectrogram);
};

#endif /* _FFT_H_ */