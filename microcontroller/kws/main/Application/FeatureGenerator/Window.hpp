#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "stdint.h"
#include "Configuration.hpp"

class Window
{
    public:
    
        Window();
        void apply(int16_t* input, float* output);
        
    private:
        float data[WINDOW_SIZE];
};

#endif /* _WINDOW_H_ */