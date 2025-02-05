#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "stdint.h"
#include "stdio.h"

class Command
{
    protected:
        const char* commandName;
        int historySize;
        float* history;
        float threshold;
        int savingCounter = 0;
        float latestAverage = 0.0;

    public:
        Command(const char* commandName, int historySize = 1, float threshold = 0.7);
        const char* getName(void);
        void giveResult(float value);
        bool isRecognized(void);
        float getAverage(void);
        virtual void execute(void) = 0;
};



#endif /* _COMMAND_H_ */
