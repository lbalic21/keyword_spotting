#include "PrintCommand.hpp"

void PrintCommand::execute(void)
{
    printf("Heard: %s (%f %%)\n", getName(), getAverage());
}
