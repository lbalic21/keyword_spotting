#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "stdint.h"
#include "Configuration.hpp"

class Window
{
    public:
    
        int16_t data[WINDOW_SIZE];
        Window();
};

#endif /* _WINDOW_H_ */