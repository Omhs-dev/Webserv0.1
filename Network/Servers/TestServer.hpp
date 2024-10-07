#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "Server.hpp"
#include <unistd.h>
#include <cstring>
#include <string.h>

class TestServer: public Server
{
	public:
		TestServer(int port);
		void handler();
		void responder(std::string reqtype, std::string conttype, \
					const char *headrs, const char *cont);
		void run();
};

#endif