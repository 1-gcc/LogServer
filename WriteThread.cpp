//  WriteThread.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <unistd.h>
#include <sys/time.h>

#include <time.h>

#include <errno.h>

#include "WriteThread.h"
#include "Msg.h"

WriteThread::WriteThread()
{
	thread = 0;
}

bool WriteThread::create()
{
	pthread_create(&thread, 0, WriteThread::threadFun, this);
	return true;
}

void * WriteThread::threadFun(void * p)
{
	if (p)
		((WriteThread *)p)->writeLoop();
}

void WriteThread::push(Msg&msg)
{
	queue.push(msg);
}
bool WriteThread::open()
{
	fpLog = fopen(fileName.c_str(), "a");
	if(!fpLog)
		return false;
	return true;
}
bool WriteThread::close()
{
	if (fpLog)
		fclose(fpLog);
	return true;
}
bool WriteThread::writeLoop()
{
	struct timespec timespec;
	struct timespec rest;
	timespec.tv_sec = 0;
	rest.tv_nsec = 0;
	rest.tv_sec = 0;
	for (;;)
	{
		sleep(0);
		if (!queue.size())
		{
			timespec.tv_nsec = (1000 * 1000 * 20) - rest.tv_nsec;
			int ret = nanosleep(&timespec,&rest);
			if (!ret)
			{
				rest.tv_nsec = 0;
				continue;
			}
			if (ret == -1)
			{
				switch (errno)
				{
				case EINTR:
					continue;
				}
				return false;
			}
		}
		else
		{
			Msg msg = queue.front();
			sMsg smsg = (sMsg)msg;
			writeIt(msg);
			queue.pop();

		}
	}
	return true;
}
bool WriteThread::writeIt(Msg&msg)
{
	struct tm * tmbuf;
	struct tm result;
	char buffer[128];
	char buffer2[128];
	time_t timer = (time_t)msg.timer;
	tmbuf = gmtime_r(&timer,&result);
	strftime(buffer, sizeof(buffer), "%Y%m%d %H%M%S", tmbuf);
	sprintf(buffer2, "%04d:%04d", msg.pid, (int)msg.threadid);
	try
	{
		fprintf(fpLog, "\n%s.%02d:%s:%3s:", buffer, msg.hundredth, buffer2,msg.level);
		fprintf(fpLog, "%s", msg.message.c_str());
		fflush(fpLog);
	}
	catch (...)
	{
		fprintf(stderr, "error logger write");
	}
	return true;
}
WriteThread::~WriteThread()
{
	close();
}
