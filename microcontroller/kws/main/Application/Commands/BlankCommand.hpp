#ifndef _BLANK_COMMAND_H_
#define _BLANK_COMMAND_H_

#include "Command.hpp"

class BlankCommand : public Command
{
    public:
        BlankCommand(const char* commandName, int historySize = 1, float threshold = 0.7) : Command(commandName, historySize, threshold) {}
        virtual void execute(void);
};

#endif /* _BLANK_COMMAND_H_ */