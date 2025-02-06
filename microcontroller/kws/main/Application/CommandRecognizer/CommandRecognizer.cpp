#include "CommandRecognizer.hpp"

bool CommandRecognizer::recognize(float* outputData)
{
    for(int i = 0; i < commandCount; i++)
    {
        commands[i]->giveResult(outputData[i]);
        if(commands[i]->isRecognized() && (((esp_timer_get_time() - lastCommandInvoke[i]) / 1000) >= COOL_OF_PERIOD_MS))
        {
            commands[i]->execute();
            commands[i]->resetHistory();
            lastCommandInvoke[i] = esp_timer_get_time();
            return true;
        }
    }
    return false;
}

bool CommandRecognizer::addCommand(Command* command)
{
    if(commandCount < MAX_COMMANDS)
    {
        commands[commandCount++] = command;
        printf("Added %s to recognizer\n", command->getName());
        return true;
    }
    return false;
}

uint8_t CommandRecognizer::getNumOfCommands(void)
{
    printf("Number of commands: %d\n", commandCount);
    return commandCount;
}

