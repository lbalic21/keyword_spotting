#include "CommandRecognizer.hpp"

bool CommandRecognizer::recognize(int numberOfClasses, float* outputData)
{
    if(((esp_timer_get_time() - lastRecognizeTime) / 1000) < COOL_OF_PERIOD_MS)
    {
        //printf("COOL-OFF PERIOD\n");
        return false;;
    }
    for(uint32_t i = 0; i < numberOfClasses; i++)
    {
        if(outputData[i] > ACTIVATION_THRESHOLD)
        {
            invokeCommand(i, outputData[i]);
            lastRecognizeTime = esp_timer_get_time();
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

void CommandRecognizer::invokeCommand(uint32_t commandIndex, float probability)
{
    if(commandIndex < commandCount)
    {
        commands[commandIndex]->execute(probability);
    }
}

void CommandRecognizer::getNumOfCommands()
{
    printf("Number of commands: %d\n", commandCount);
}

