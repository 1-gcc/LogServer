//  LogThread.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <time.h>
#include <string.h>
#include <errno.h>

#include "LogThread.h"
#include "MsgInit.h"
#define MAX_MESSAGE_SIZE (128*1024)



LogThread::LogThread(Socket & socket, Writer * writer) : socket(socket),writer(writer)
{
	thread = 0;
	app = 0;
}

LogThread::~LogThread()
{
	
}
#pragma pack(1)
#pragma push
struct sHeader
{
	ushort length;
	ushort msgtype;
};
struct sResponse
{
public:
	sResponse(ushort app) :app(app)
	{
		length = sizeof(*this);
		msgtype = 3;
	}
	ushort length;
	ushort msgtype;
	ushort app;
};
#pragma pop

bool recv_ushort(Socket & socket,ushort&length)
{
	ushort lengthBuf;
	for (;;)
	{
		size_t ret = recv(socket.socket, &lengthBuf, sizeof(lengthBuf), 0);
		if (ret == (size_t)-1)
		{
			switch (errno)
			{
			case EINTR:
				break;
			default:
				socket.close();
				return false;
			}
			continue;
		}
		else if (!ret || ret != sizeof(lengthBuf))
		{
			socket.close();
			return false;
		}
		length = ntohs(lengthBuf);
		return true;
	}
}
bool recv_string(Socket& socket, ushort length, std::string&str)
{
	size_t toRead = length;
	size_t ret = 0;
	size_t readBytes = 0;
	char * buffer = new char[toRead + 1];
	for (;;)
	{
		ret = recv(socket.socket, buffer + readBytes, toRead, 0);
		if (ret == (size_t)-1)
		{
			switch (errno)
			{
			case EINTR:
				break;
			default:
				socket.close();
				return false;
			}
			continue;
		}
		else if (!ret)
		{
			socket.close();
			return false;
		}
		toRead -= ret;
		readBytes += ret;
		if (!toRead)
		{
			buffer[readBytes] = 0;
			str = buffer;
			return true;
		}
	}
	return false;
}
bool recv_initmsg(Socket & socket, MsgInit&initmsg)
{

	ushort length = 0;
	if (!recv_ushort(socket, length))
		return false;
	if (!recv_string(socket, length,initmsg.user))
		return false;
	if (!recv_ushort(socket, length))
		return false;
	if (!recv_string(socket, length, initmsg.app))
		return false;
	if (!recv_ushort(socket, length))
		return false;
	if (!recv_string(socket, length, initmsg.file))
		return false;
}
bool recv_msg(Socket&socket, Msg&message)
{
	sMsg msg;
	size_t ret = recv(socket.socket, &msg, sizeof(msg), 0);
	ulong len = ntohs(msg.lengthMessage);
	if (len > 0 && len <= MAX_MESSAGE_SIZE)
	{
		char * buffer = new char[len];
		size_t toRead = len;
		size_t readBytes = 0;
		do
		{
			ret = recv(socket.socket, buffer + readBytes, toRead, 0);
			if (ret == (size_t)-1)
			{
				socket.close();
				return false;
			}
			else if (!ret)
			{
				socket.close();
				return false;
			}
			readBytes += ret;
			toRead -= ret;
		} while (toRead);
		message = Msg(msg, buffer, readBytes);

	}
}
bool send_response(Socket & socket, sResponse&response)
{
	size_t ret;
	size_t toSend = sizeof(response);
	size_t sentBytes = 0;
	for (;;)
	{
		ret = send(socket.socket, &response, toSend, 0);
		if (ret == (size_t)-1)
		{
			switch (errno)
			{
			case EINTR:
				continue;
			}
			return false;
		}
		else if (!ret)
		{
			socket.close();
			return false;
		}
		toSend -= ret;
		sentBytes += ret;
		if (toSend == 0)
			return true;
	}
	return false;
}
bool LogThread::logLoop()
{
	
	for (;;)
	{
		ulong ul = 0;
		Msg message;
		MsgInit initmsg;
		char * buffer = NULL;
		ushort length = 0;
		ushort msgtype;
		if (!recv_ushort(socket, length))
			return false;
		if (!recv_ushort(socket, msgtype))
			return false;
		if (msgtype == 1)
		{
			if (!recv_initmsg(socket, initmsg))
			{
				return false;
			}
			app = writer->newApp(initmsg.user,initmsg.app,initmsg.file);
			sResponse response(app);
			send_response(socket,response);
		}
		else if (msgtype == 2)
		{
			if (!recv_msg(socket, message))
				return false;
			writer->getWriteThread(app)->push(message);
		}
		else
		{
			socket.close();
			return false;
		}

	}
	return false;
}

bool LogThread::create()
{
	pthread_create(&thread,0,LogThread::threadFun,this)  ;
	return false;
}

void * LogThread::threadFun(void * p)
{
	if(p)
		((LogThread *)p)->logLoop();
}

