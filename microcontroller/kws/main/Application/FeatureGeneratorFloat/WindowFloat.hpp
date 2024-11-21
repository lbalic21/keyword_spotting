#ifndef _WINDOW_FLOAT_H_
#define _WINDOW_FLOAT_H_

#include "stdint.h"
#include "Configuration.hpp"

class WindowFloat
{
    public:
    
        float data[WINDOW_SIZE];
        WindowFloat();
};

#endif /* _WINDOW_H_ */