#ifndef ISENDER_H
#define ISENDER_H

#include "../Message/Message.h"

struct ISender
{
	virtual void deQRequest() = 0;
	virtual void enQRequest(Message& ms) = 0;
};


#endif