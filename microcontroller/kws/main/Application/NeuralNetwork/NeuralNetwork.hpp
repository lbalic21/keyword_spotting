#ifndef _NEURAL_NETWORK_H_
#define _NEURAL_NETWORK_H_

#include "stdbool.h"
#include "stdint.h"
#include "Configuration.hpp"
#include "esp_log.h"
#include "model.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#define TENSOR_ARENA_SIZE   30*1024

class NeuralNetwork
{
    private:
        const tflite::Model* model = nullptr;
        tflite::MicroInterpreter* interpreter = nullptr;
        TfLiteTensor* model_input = nullptr;

        uint8_t tensor_arena[TENSOR_ARENA_SIZE];
        tflite::MicroMutableOpResolver<4> resolver;
        int8_t* model_input_buffer = nullptr;

    public:
        NeuralNetwork();
        void giveFeaturesToModel(int8_t* features, size_t numberOfFeatures);
        bool invoke(int8_t* featureImage);
};

#endif /* _NEURAL_NETWORK_H_ */