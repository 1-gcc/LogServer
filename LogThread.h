//  LogThread.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include <queue>
#include <string>

#include "Socket.h"
#include "Writer.h"


class LogThread
{
public:
	LogThread(Socket & socket,Writer*writer);
	~LogThread();
	bool logLoop();
	bool create();
	static void * threadFun(void*p);
	Socket socket;
	Writer * writer;
	pthread_t thread;
	ushort app;
};

