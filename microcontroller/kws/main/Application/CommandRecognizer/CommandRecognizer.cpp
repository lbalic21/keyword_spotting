#include "CommandRecognizer.hpp"

bool CommandRecognizer::recognize(float* outputData)
{
    // saving newest result
    for(int i = 0; i < commandCount; i++)
    {
        lastResults[savingCounter][i] = outputData[i];
    }

    // increment saving counter
    incrementCounter();

    // calculating average scores
    float averageScores[commandCount];
    for(int i = 0; i < commandCount; i++)
    {
        for(int j = 0; j < N; j++)
        {
            averageScores[i] += lastResults[j][i];
        }
    }

    // see if any command score is higher than the threshold
    for(uint32_t i = 0; i < commandCount; i++)
    {
        if(averageScores[i] > ACTIVATION_THRESHOLD)
        {
            invokeCommand(i, outputData[i]);
            lastRecognizeTime = esp_timer_get_time();
            return true;
        }
    }

    // COOL_OFF period, not shure if needed
    /*
    if(((esp_timer_get_time() - lastRecognizeTime) / 1000) < COOL_OF_PERIOD_MS)
    {
        //printf("COOL-OFF PERIOD\n");
        //return false;;
    }
    */
    return false;
}

void CommandRecognizer::incrementCounter(void)
{
    savingCounter = ++savingCounter % N;
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

void CommandRecognizer::getNumOfCommands(void)
{
    printf("Number of commands: %d\n", commandCount);
}

