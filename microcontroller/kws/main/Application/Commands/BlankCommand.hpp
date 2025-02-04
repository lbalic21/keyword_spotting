#ifndef _BLANK_COMMAND_H_
#define _BLANK_COMMAND_H_

#include "Command.hpp"

class BlankCommand : public Command
{
    public:
        BlankCommand(const char* commandName) : Command(commandName) {}
        void execute(float probability);
};

#endif /* _BLANK_COMMAND_H_ */