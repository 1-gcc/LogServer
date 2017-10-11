//  Queue.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include <queue>

#include "Locker.h"
#include "WriteQueue.h"
#include "ReadQueue.h"

class Queue : public IWriteQueue, public IReadQueue
{
public:


	Queue()
	{
		lock.init();
	}
	~Queue()
	{

	}
	virtual void push(Msg&message);
	virtual Msg  front() const;
	virtual void pop();
	virtual size_t size() const ;
protected:
	Locker lock;
	std::queue<Msg> queue;
};
