#include "CommandResponder.hpp"

const char* commands[6] = 
{
    "silence",
    "left",
    "no",
    "right"
    "unknown",
    "yes",
};

void CommandResponder::respond(uint32_t command, float probability)
{
    if(command != 0 && command != 4)
    {
        printf("Heard %ld, probability %f\n", command, probability);
    }
}