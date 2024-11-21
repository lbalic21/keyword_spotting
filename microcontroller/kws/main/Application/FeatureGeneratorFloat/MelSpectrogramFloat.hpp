#ifndef _MEL_SPECTROGRAM_FLOAT_H_
#define _MEL_SPECTROGRAM_FLOAT_H_

#include "stdint.h"
#include "Configuration.hpp"

class MelSpectrogramFloat
{
    public:
        MelSpectrogramFloat() {}
        void generate(float* spectrogram, float* melSpectrogram);

    private:
        float melPoints[NUMBER_OF_MEL_BINS + 2];

        float hzToMel(float hz);
        float melToHz(float mel);

};

#endif /* _MEL_SPECTROGRAM_FLOAT_H_ */