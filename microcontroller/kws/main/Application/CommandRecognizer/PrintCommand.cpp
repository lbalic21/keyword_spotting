#include "PrintCommand.hpp"

void PrintCommand::execute(float probability)
{
    printf("Heard: %s (%f %%)\n", this->name, probability * 100);
}
