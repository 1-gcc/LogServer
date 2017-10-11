//  LogServer.cpp
// (C) Copyright 2017 by Martin Brunn  see License.txt for details
//
#include <set>
#include <dlfcn.h>

#include "LogServer.h"
#include "Socket.h"
#include "LogThread.h"
#include "WriteThread.h"
#include "Writer.h"
#include "IDOM.h"
#include "IWSConverter.h"


#ifndef RTLD_NOW
#define RTLD_NOW 2
#define RTLD_GLOBAL 0x0100
#endif
using namespace Xelite;
using namespace cardac::converter;
class Config
{

public:
	std::set<ushort> setPorts;
	void addPort(ushort port)
	{
		setPorts.insert(port);
	}
};
class ConfigReader
{
public:
	cardac::converter::CreateConverter createConverter;
	cardac::converter::DeleteConverter deleteConverter;
	//void(*vminfo)();
	//Xelite::ParseXMLUTF8Stream parseXMLUTF8Stream;
	Xelite::ParseXMLUTF8File parseXMLUTF8File;
	Xelite::DeleteDOM deleteDOM;
	void * lib;
	ConfigReader()
	{
		lib = dlopen("xelite.so", RTLD_NOW | RTLD_GLOBAL);
		if (lib == NULL)
		{
			char* err = dlerror();
			fprintf(stderr, "dlopen error %s\n", err);
			fprintf(stderr, "CAN'T READ CONFIGURATION FILE\n");
		}
	}
	~ConfigReader()
	{
		if (lib)
			dlclose(lib);
	}
	bool init()
	{
		if (!lib)
			return false;
		createConverter = (CreateConverter)dlsym(lib, "createConverter");
		if (!createConverter)
		{
			fprintf(stderr, "createConverter missing");
			return false;
		}
		deleteConverter = (DeleteConverter)dlsym(lib, "deleteConverter");
		if (!deleteConverter)
		{
			fprintf(stderr, "deleteConverter missing");
			return false;
		}
		parseXMLUTF8File = (ParseXMLUTF8File)dlsym(lib, "parseXMLUTF8File");
		if (!parseXMLUTF8File)
		{
			fprintf(stderr, "parseXMLUTF8File missing");
			return false;
		}
		deleteDOM = (DeleteDOM)dlsym(lib, "deleteDOM");
		if (!deleteDOM)
		{
			fprintf(stderr, "deleteDOM missing");
			return false;
		}

	}
	bool read(const char * file,Config&cfg)
	{
		IDOM * pDOM = parseXMLUTF8File(file, false);
		if (!pDOM)
			return false;
		IElementPtr root = pDOM->getDocument()->getRoot();
		if (root->getName() == L"ServerConfig")
		{
			IElement::iterator i = root->findFirstChild(L"Config");
			if (i != root->end())
			{
				IElementPtr config(*i) ;
				IElement::iterator ic = config->findFirstChild(L"Listen");
				for (; ic != config->end(); ++ic)
				{
					int port = 0;
					if ((*ic)->getAttributeInt(L"port", port))
					{
						cfg.addPort((ushort)port);
					}
				}
			} // end if i != root end
		} // end if root name
		deleteDOM(pDOM);
	} // end read()
};
LogServer::LogServer()
{

}


LogServer::~LogServer()
{
}

bool LogServer::initServer(int argc, char ** argv)
{
	return true;
}

bool LogServer::runServer()
{
	Socket socket;
	Config config;
	do
	{
		ConfigReader cfgReader;
		if (!cfgReader.init())
			return false;
		if (!cfgReader.read("logserver.xml", config))
			return false;
	} while (0);

	std::set<ushort>::iterator i = config.setPorts.begin();
	if (i == config.setPorts.end())
		return false;

	ushort port = *i;
	if (!socket.create())
	{
		fprintf(stderr, "error socket\n");
		return false;
	}
	if (!socket.listen(port))
	{
		fprintf(stderr, "error listen 7777\n");
		return false;
	}
	if (socket.setSocketBlocking() == -1)
	{
		fprintf(stderr, "error setBlocking\n");
		return false;
	}
	Writer * writer = new Writer();
	if (!writer)
	{
		fprintf(stderr, "no writer\n");
		return false;
	}
	fprintf(stderr, "listen on port %u",port);
	for (;;)
	{
		Socket conn;
		if (!conn.create())
		{
			fprintf(stderr, "error conn.create\n");

			return false;
		}
		if (!conn.accept(socket))
			continue;
		LogThread * thread = new LogThread(conn,writer);
		if (thread)
		{
			thread->create();
		}
	}
	return true;
}
