#ifndef _MEL_SPECTROGRAM_
#define _MEL_SPECTROGRAM_

#include "stdint.h"
#include "Configuration.hpp"

class MelFilterbank
{
    public:
        MelFilterbank() {}
        void generate(uint32_t* spectrogram, int32_t* melSpectrogram);

    private:
        float melPoints[NUMBER_OF_MEL_BINS + 2];
};

#endif /* _MEL_SPECTROGRAM_ */