#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>

#include "../Sockets/headerSocket.hpp"

class Server
{
	private:
		int _new_socket;
		ListeningSocket *socket;
		char _request_buffer[30000];

		virtual void run() = 0;
		virtual void handler() = 0;
		virtual void responder(std::string reqtype, std::string conttype, const char *headers, const char *content) = 0;

	public:
		Server(int domain, int service, int protocol, int port, u_long interf, int bklog);
		
		ListeningSocket *get_socket();
		
		int getNewSocket();
		void setNewSocket(int new_socket);
		char *getRequestBuffer();
		void accepter();
};

#endif