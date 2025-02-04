#ifndef _PRINT_COMMAND_H_
#define _PRINT_COMMAND_H_

#include "Command.hpp"

class PrintCommand : public Command
{
    public:
        PrintCommand(const char* commandName) : Command(commandName) {}
        virtual void execute(float probability);
};

#endif /* _PRINT_COMMAND_H_ */