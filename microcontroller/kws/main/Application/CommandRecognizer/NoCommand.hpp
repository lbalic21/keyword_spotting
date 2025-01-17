#include "Command.hpp"

class NoCommand : public Command
{
    public:
        NoCommand(const char* commandName) : Command(commandName) {}
    
        void execute(float probability);
};