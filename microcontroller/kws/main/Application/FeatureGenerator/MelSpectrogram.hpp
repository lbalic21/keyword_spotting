#ifndef _MEL_SPECTROGRAM_
#define _MEL_SPECTROGRAM_

#include "stdint.h"
#include "Configuration.hpp"

class MelSpectrogram
{
    public:
        MelSpectrogram() {}
        void generate(uint32_t* spectrogram, int32_t* melSpectrogram);

    private:
        float melPoints[NUMBER_OF_MEL_BINS + 2];

        float hzToMel(float hz);
        float melToHz(float mel);

};

#endif /* _MEL_SPECTROGRAM_ */