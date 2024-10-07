#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include "BindingSocket.hpp"

class ListeningSocket: public BindingSocket
{
	private:
		int _backlog;
		int _listening;
		
	public:
		ListeningSocket(int domain, int service, int protocol, int port, u_long interf, int bklog);
		virtual ~ListeningSocket();  

		void start_listening(int sock, int backlog);
};
