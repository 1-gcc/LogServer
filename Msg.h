//  Msg.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once

#include <sys/types.h>
#include <pthread.h>
#include <string>

#pragma push
#pragma pack(1)

typedef __U32_TYPE portable_time_t;

struct sMsg
{
	ushort request;
	portable_time_t timer;
	ushort hundredth;
	char level[4];
	pid_t pid;
	pthread_t threadid;
	ushort lengthMessage;
};
#pragma pop


class Msg
{
public:
	ushort request;
	portable_time_t timer;
	ushort hundredth;
	char level[4];
	pid_t pid;
	pthread_t threadid;
	ushort lengthMessage;
	std::string message;
	Msg();
	Msg(const sMsg & msg, const char*buffer, size_t len);
	operator sMsg() const;
};

