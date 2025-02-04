#ifndef _DCT_HPP_H_
#define _DCT_HPP_H_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Configuration.hpp"

class DCT 
{
    public:
        DCT();
        void compute(const float *input, float *output);

    private:
        float coefficients[NUMBER_OF_MEL_BINS * (NUMBER_OF_MFCCS + 1)];
};

#endif /* _DCT_HPP_H_ */