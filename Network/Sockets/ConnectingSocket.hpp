#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "Socket.hpp"

class ConnectingSocket: public Socket
{
	public:
		ConnectingSocket(int domain, int service, int protocol, int port, u_long interf);
		// ~ConnectingSocket();
		
		int connect_network(int sock, struct sockaddr_in address);
};
