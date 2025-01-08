#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "stdint.h"
#include "Configuration.hpp"

template <typename T>
class Window
{
    public:
    
        T data[WINDOW_SIZE];
        Window();
};

#endif /* _WINDOW_H_ */