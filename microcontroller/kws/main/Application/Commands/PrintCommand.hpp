#ifndef _PRINT_COMMAND_H_
#define _PRINT_COMMAND_H_

#include "Command.hpp"

class PrintCommand : public Command
{
    public:
        PrintCommand(const char* commandName, int historySize, float threshold) : Command(commandName, historySize, threshold) {}
        virtual void execute(void);
};

#endif /* _PRINT_COMMAND_H_ */