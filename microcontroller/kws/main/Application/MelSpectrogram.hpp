#ifndef _MEL_SPECTROGRAM_
#define _MEL_SPECTROGRAM_

#include "stdint.h"
#include "Configuration.hpp"

class MelSpectrogram
{
    private:
        int16_t data[NUMBER_OF_MEL_BINS];

    public:
        MelSpectrogram() {}
        void generate(int16_t* spectrogram, int16_t* melSpectrogram);

};

#endif /* _MEL_SPECTROGRAM_ */