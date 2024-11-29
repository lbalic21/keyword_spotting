#ifndef _MODEL_H_
#define _MODEL_H_

#include "stdbool.h"
#include "stdint.h"

class Model
{

    public:

        Model() {}
        bool invoke(int8_t* featureImage);
};

#endif /* _MODEL_H_ */