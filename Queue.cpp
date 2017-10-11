//  Queue.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//

#include "Queue.h"

void Queue::push(Msg & message)
{
	lock.lock_wait();
	queue.push(message);
	lock.lock_release();
}

Msg Queue::front() const
{
	return queue.front();
}

void Queue::pop()
{
	lock.lock_wait();
	queue.pop();
	lock.lock_release();
}
size_t Queue::size() const
{
	return queue.size();
}