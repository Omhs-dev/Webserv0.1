#pragma once

#include "Socket.hpp"

class BindingSocket: public Socket
{
	public:
		BindingSocket(int domain, int service, int protocol, int port, u_long interf);
		// ~BindingSocket();
		
		int connect_network(int sock, struct sockaddr_in address);
};
