#ifndef _WINDOW_FLOAT_H_
#define _WINDOW_FLOAT_H_

#include "stdint.h"
#include "Configuration.hpp"

class WindowFloat
{
    public:
    
        WindowFloat();
        void apply(int16_t* input, float* output);
        
    private:
        float data[WINDOW_SIZE];
};

#endif /* _WINDOW_H_ */