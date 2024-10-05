#ifndef SERVER_HPP
#define SERVER_HPP

#include "../headerNetwork.hpp"

class Server
{
	private:
		ListeningSocket *socket;
		virtual void accepter() = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;

	public:
		Server(int domain, int service, int protocol, int port, u_long interf, int bklog);
		
		virtual void run() = 0;
		ListeningSocket *get_socket();
};

#endif