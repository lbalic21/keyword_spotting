#include "PrintCommand.hpp"

void PrintCommand::execute(float probability)
{
    printf("Heard: %s (%f) %%\n", name, probability * 100);
}
