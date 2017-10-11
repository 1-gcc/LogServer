//  Writer.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "Writer.h"



Writer::Writer()
{
	nextAppId = 1;
}


Writer::~Writer()
{
}

WriteThread * Writer::getWriteThread(ushort app)
{
	MapThreads::iterator i = writeThreads.find(app);
	if(i == writeThreads.end())
		return NULL;

	return i->second;
}

ushort Writer::newApp(const std::string & user, const std::string & app, const std::string & file)
{
	const char* pattern = "%s/%s/%s";
	size_t len = strlen(pattern) + user.size() + app.size() + file.size();
	char * buffer = new char[len + 1];
	sprintf(buffer, pattern, user.c_str(), app.c_str(), file.c_str());
	std::string name = buffer;
	delete buffer;
	ushort appid = 0;
	MapAppIds::iterator j = appIds.find(name);
	if (j == appIds.end())
	{
		return newApp(name);
	}
	return j->second;
}

ushort Writer::newApp(const std::string & name)
{
	ushort appId = nextAppId++;
	appIds.insert(std::make_pair(name, appId));
	writeThreads.insert(std::make_pair(appId, new WriteThread()));
	return ushort();
}
