//  WriteQueue.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include "Msg.h"

class IWriteQueue
{
public:
	virtual void push(Msg&message) = 0;
};
