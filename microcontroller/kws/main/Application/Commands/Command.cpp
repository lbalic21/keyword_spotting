#include "Command.hpp"

Command::Command(const char* commandName, int historySize, float threshold)
{
    this->commandName = commandName;
    this->historySize = historySize;
    this->threshold = threshold;
    this->history = (float*) malloc(sizeof(float) * historySize);
    for(int i = 0; i < historySize; i++)
    {
        this->history[i] = 0.0;
    }
}

const char* Command::getName(void)
{
    return commandName;
}

void Command::giveResult(float value)
{
    history[savingCounter] = value;
    savingCounter++;
    savingCounter = savingCounter % historySize;
}

bool Command::isRecognized(void)
{
    float averageScore = 0.0;
    for(int i = 0; i < historySize; i++)
    {
        averageScore += history[i];
    }
    averageScore /= historySize;

    this->latestAverage = averageScore;

    if(averageScore > this->threshold)
    {
        return true;
    }
    return false;
}

float Command::getAverage(void)
{
    return latestAverage;
}