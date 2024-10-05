#include "Server.hpp"

Server::Server(int domain, int service, int protocol, int port, u_long interf, int bklog)
{
	socket = new ListeningSocket(domain, service, protocol, port, interf, bklog);
}

ListeningSocket *Server::get_socket()
{
	return (socket);
}