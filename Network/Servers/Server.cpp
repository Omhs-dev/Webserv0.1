#include "Server.hpp"

/**
 * @brief Constructs a Server object.
 *
 * This constructor initializes a Server object with the specified parameters.
 *
 * @param domain The domain of the server socket.
 * @param service The service type of the server socket.
 * @param protocol The protocol of the server socket.
 * @param port The port number of the server socket.
 * @param interf The interface address of the server socket.
 * @param bklog The maximum length of the queue of pending connections.
 */
Server::Server(int domain, int service, int protocol, int port, u_long interf, int bklog)
{
	socket = new ListeningSocket(domain, service, protocol, port, interf, bklog);
}

char *Server::getRequestBuffer()
{
	return (_request_buffer);
}

int Server::getNewSocket()
{
	return (_new_socket);
}

void Server::setNewSocket(int new_socket)
{
	_new_socket = new_socket;
}

void Server::accepter()
{
	struct sockaddr_in client_addr = get_socket()->get_address();
	int addrlen = sizeof(client_addr);
	_new_socket = accept(get_socket()->get_socket(), (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
	read(_new_socket, _request_buffer, 30000);
}

ListeningSocket *Server::get_socket()
{
	return (socket);
}