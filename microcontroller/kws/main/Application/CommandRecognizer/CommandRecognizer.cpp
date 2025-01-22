#include "CommandRecognizer.hpp"

bool CommandRecognizer::recognize(float* outputData)
{
    //printf("Saving counter: %d\n", savingCounter);
    // saving newest result
    for(int i = 0; i < commandCount; i++)
    {
        lastResults[savingCounter][i] = outputData[i];
        //printf("OutputData[%d]=%f\n", i, outputData[i]);
    }

    // increment saving counter
    incrementCounter();

    // calculating average scores
    float averageScores[commandCount] = {0};

    for(int i = 0; i < commandCount; i++)
    {
        for(int j = 0; j < WWW; j++)
        {
            averageScores[i] += lastResults[j][i];
        }
        averageScores[i] /= WWW;
        //printf("Avg[%d]=%f\n", i, averageScores[i]);
    }
    //printf("\n");

    // see if any command score is higher than the threshold
    for(uint32_t i = 0; i < commandCount; i++)
    {
        if((outputData[i] > ACTIVATION_THRESHOLD))// && (((esp_timer_get_time() - lastCommandInvoke[i]) / 1000) >= COOL_OF_PERIOD_MS))
        {
            invokeCommand(i, outputData[i]);
            lastCommandInvoke[i] = esp_timer_get_time();
            return true;
        }
    }

    return false;
}

void CommandRecognizer::incrementCounter(void)
{
    savingCounter++;
    savingCounter = savingCounter % WWW;
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

