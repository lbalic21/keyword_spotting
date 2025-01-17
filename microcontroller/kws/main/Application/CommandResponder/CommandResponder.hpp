#ifndef _COMMAND_RESPONDER_H_
#define _COMMAND_RESPONDER_H_

#include "stdio.h"
#include "stdbool.h"

class CommandResponder
{
    public:
        void respond(uint32_t command, float probability);
};

#endif /* _COMMAND_RESPONDER_H_ */