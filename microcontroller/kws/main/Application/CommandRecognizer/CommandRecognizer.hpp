#ifndef _COMMAND_RECOGNIZER_H_
#define _COMMAND_RECOGNIZER_H_

#include "stdint.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "Configuration.hpp"
#include "Commands/Command.hpp"

class CommandRecognizer
{
    private:
        Command* commands[MAX_COMMANDS];
        uint8_t commandCount = 0;
        int64_t lastCommandInvoke[MAX_COMMANDS] = {0};

    public:
        bool addCommand(Command* command);
        uint8_t getNumOfCommands(void);
        bool recognize(float* outputData);
};

#endif /* _COMMAND_RECOGNIZER_H_ */