#ifndef DCT_HPP
#define DCT_HPP

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Configuration.hpp"

class DCT {
public:
    DCT();

    // Perform DCT on input array
    void compute(const float *input, float *output);

private:

    float coefficients[NUMBER_OF_MEL_BINS * NUMBER_OF_MFCCS];
};

#endif // DCT_HPP