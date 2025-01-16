#ifndef _COMMAND_RECOGNIZER_H_
#define _COMMAND_RECOGNIZER_H_

#include "stdint.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "Configuration.hpp"

class CommandRecognizer
{
    private:
        int64_t lastRecognizeTime = 0;

    public:
        bool recognize(int numberOfClasses, float* outputData);
        uint32_t recognizedCommand;
        float probability;
};

#endif /* _COMMAND_RECOGNIZER_H_ */