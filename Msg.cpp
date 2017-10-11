//  Msg.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <arpa/inet.h>
#include <string.h>

#include "Msg.h"

Msg::Msg()
{
	request = 0;
	timer = 0;
	hundredth = 0;
	pid = 0;
	threadid = 0;
	lengthMessage = 0;
	memset(level, 0, sizeof(level));
}
Msg::Msg(const sMsg & msg, const char*buffer, size_t len)
{
	request = ntohs(msg.request);
	timer = ntohl(msg.timer);
	hundredth = ntohs(msg.hundredth);
	memcpy(level, msg.level, sizeof(level));
	pid = ntohl(msg.pid);
	threadid = ntohl(msg.threadid);
	lengthMessage = ntohs(msg.lengthMessage);
	message = std::string(buffer, len);
}
Msg::operator sMsg() const
{
	sMsg msg;
	msg.request = htons(request);
	msg.timer = htonl(timer);
	msg.hundredth = htons(hundredth);
	memcpy(msg.level, level, sizeof(msg.level));
	msg.pid = htonl(pid);
	msg.threadid = htonl(threadid);
	msg.lengthMessage = htons(lengthMessage);
	return msg;
}
