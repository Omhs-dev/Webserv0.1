#pragma once

#include "BindingSocket.hpp"

class ListeningSocket: public BindingSocket
{
	private:
		int _backlog;
		int _listening;
		
	public:
		ListeningSocket(int domain, int service, int protocol, int port, u_long interf, int bklog);
		// ~ListeningSocket();

		void start_listening();
};
