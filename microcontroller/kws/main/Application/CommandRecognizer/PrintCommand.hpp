#include "Command.hpp"

class PrintCommand : public Command
{
public:
    PrintCommand(const char* commandName) : Command(commandName) {}

    virtual void execute(float probability);
};