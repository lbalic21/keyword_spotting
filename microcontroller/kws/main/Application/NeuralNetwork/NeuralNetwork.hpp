#ifndef _NEURAL_NETWORK_H_
#define _NEURAL_NETWORK_H_

#include "stdbool.h"
#include "stdint.h"
#include "model.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

class NeuralNetwork
{
    private:
        const tflite::Model* model = nullptr;

    public:

        NeuralNetwork();
        bool invoke(int8_t* featureImage);
};

#endif /* _NEURAL_NETWORK_H_ */