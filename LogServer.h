//  LogServer.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include "Queue.h"
class LogServer
{
	
public:
	
	LogServer();
	~LogServer();
	bool initServer(int argc, char**argv);
	bool runServer();
protected:
	Queue queue;
};

