//  main.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <stdio.h>

#include "LogServer.h"


int main(int argc, char**argv)
{
	LogServer server;

	if (server.initServer(argc, argv))
		return server.runServer() ? 0 : 2 ;
	return 1;
}