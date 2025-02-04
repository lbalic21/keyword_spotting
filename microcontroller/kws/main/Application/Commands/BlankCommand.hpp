#include "Command.hpp"

class BlankCommand : public Command
{
    public:
        BlankCommand(const char* commandName) : Command(commandName) {}
        void execute(float probability);
};