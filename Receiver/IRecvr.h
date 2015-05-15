#ifndef IRECVR_H
#define IRECVR_H

#include "../Message/Message.h"

struct IReceiver
{
	virtual void deQRequest() = 0;
	virtual Message deQSingleRequest() = 0;
	virtual void enQRequest(Message& ms) = 0;
};


#endif