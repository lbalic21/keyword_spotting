#ifndef _COMMAND_RECOGNIZER_H_
#define _COMMAND_RECOGNIZER_H_

#include "stdint.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "Configuration.hpp"
#include "Command.hpp"

class CommandRecognizer
{
    private:
        int64_t lastRecognizeTime = 0;
        Command* commands[MAX_COMMANDS];
        uint8_t commandCount = 0;

    public:
        bool recognize(int numberOfClasses, float* outputData);
        bool addCommand(Command* command);
        void invokeCommand(uint32_t commandIndex, float probability);
        void getNumOfCommands();
};

#endif /* _COMMAND_RECOGNIZER_H_ */