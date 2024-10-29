#ifndef _MEL_SPECTROGRAM_
#define _MEL_SPECTROGRAM_

#include "stdint.h"
#include "Configuration.hpp"

class MelSpectrogram
{
    public:
        MelSpectrogram() {}
        void generate(uint32_t* spectrogram, uint32_t* melSpectrogram);

    private:
        uint32_t melPoints[NUMBER_OF_MEL_BINS + 2];
        static const int FIXED_POINT_FRACTIONAL_BITS = 16; // Number of bits for the fractional part
        static const uint32_t FIXED_POINT_ONE = (1 << FIXED_POINT_FRACTIONAL_BITS);

        float hzToMel(float hz);
        float melToHz(float mel);

};

#endif /* _MEL_SPECTROGRAM_ */