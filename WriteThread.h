//  WriteThread.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include <stdio.h>
#include <pthread.h>
#include <string>

#include "Queue.h"

class WriteThread
{
public:
	WriteThread();
	~WriteThread();
	bool create();
	bool writeLoop();
	bool writeIt(Msg & msg);
	static void * threadFun(void * p);
	void push(Msg&msg);
	bool open();
	bool close();
protected:
	Queue queue;
	pthread_t thread;
	FILE * fpLog;
	std::string fileName;
};

