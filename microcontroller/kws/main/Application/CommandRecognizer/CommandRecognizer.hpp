#ifndef _COMMAND_RECOGNIZER_H_
#define _COMMAND_RECOGNIZER_H_

#include "stdint.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "Configuration.hpp"
#include "Command.hpp"

#define WWW      5

class CommandRecognizer
{
    private:
        Command* commands[MAX_COMMANDS];
        uint8_t commandCount = 0;

        float lastResults[WWW][MAX_COMMANDS] = {0};
        int savingCounter = 0;

        int64_t lastCommandInvoke[MAX_COMMANDS] = {0};

    public:
        bool recognize(float* outputData);
        void incrementCounter(void);

        bool addCommand(Command* command);
        void invokeCommand(uint32_t commandIndex, float probability);
        void getNumOfCommands(void);
};

#endif /* _COMMAND_RECOGNIZER_H_ */