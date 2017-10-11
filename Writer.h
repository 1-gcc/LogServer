//  Writer.h
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#pragma once
#include <sys/types.h>
#include <map>
#include "WriteThread.h"
class Writer
{
public:
	typedef std::map<ushort, WriteThread*> MapThreads;
	typedef std::map<std::string, ushort> MapAppIds;
	Writer();
	~Writer();
	WriteThread*getWriteThread(ushort app);
	ushort newApp(const std::string&user ,const std::string&app, const std::string&file);
	ushort newApp(const std::string&name);
protected:
	MapThreads writeThreads;	
	MapAppIds appIds;
	ushort nextAppId;
};

