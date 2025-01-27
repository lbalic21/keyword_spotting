#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "stdio.h"

class Command
{
    protected:
        const char* name;
        int numberOfNetworkInvokings = 0;
        int numberOfRecognitions = 0;

    public:
        Command(const char* commandName) : name(commandName) {}
        virtual ~Command() {}
        
        const char* getName(void)
        {
            return name;
        }
        virtual void execute(float probability) = 0;
};



#endif /* _COMMAND_H_ */
