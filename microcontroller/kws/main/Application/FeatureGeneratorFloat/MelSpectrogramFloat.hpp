#ifndef _MEL_SPECTROGRAM_FLOAT_H_
#define _MEL_SPECTROGRAM_FLOAT_H_

#include "stdint.h"
#include "Configuration.hpp"

class MelSpectrogramFloat
{
    public:
        MelSpectrogramFloat();
        void generate(float* spectrogram, float* melSpectrogram);

    private:
        float lowerMel;
        float upperMel;
        float melStep;
        float hzPerBin;
        float melPoints[NUMBER_OF_MEL_BINS + 2];

};

#endif /* _MEL_SPECTROGRAM_FLOAT_H_ */