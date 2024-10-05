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

ListeningSocket *Server::get_socket()
{
	return (socket);
}