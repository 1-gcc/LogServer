//  ReadQueue.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include "Msg.h"

class IReadQueue
{
public:
	virtual Msg  front() const = 0;
	virtual void pop() = 0;
	virtual size_t size() const = 0;
};

