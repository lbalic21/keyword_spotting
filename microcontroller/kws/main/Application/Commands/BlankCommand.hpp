#ifndef _BLANK_COMMAND_H_
#define _BLANK_COMMAND_H_

#include "Command.hpp"

class BlankCommand : public Command
{
    public:
        BlankCommand(const char* commandName, int historySize, float threshold) : Command(commandName, historySize, threshold) {}
        virtual void execute(void);
};

#endif /* _BLANK_COMMAND_H_ */