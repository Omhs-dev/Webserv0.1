#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "Server.hpp"
#include <unistd.h>
#include <cstring>
#include <string.h>

class TestServer: public Server
{
	private:
		char _buffer[30000] = {0};
		int _new_socket;
		void accepter();
		void handler();
		void responder();

	public:
		TestServer();
		void run();
};

#endif