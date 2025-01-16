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
            recognizedCommand = i;
            probability = outputData[i];
            lastRecognizeTime = esp_timer_get_time();
            return true;
        }
    }
    return false;
}

