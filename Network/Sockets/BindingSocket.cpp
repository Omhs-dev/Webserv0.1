#include "BindingSocket.hpp"

// Constructor for the BindingSocket class
BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interf)
				: Socket(domain, service, protocol, port, interf)
{
	connect_network(get_socket(), get_address());
	check_connection(get_connection());
}

// Connects the socket to the network
int BindingSocket::connect_network(int sock, struct sockaddr_in address)
{
	return (bind(sock, (struct sockaddr *)&address, sizeof(address)));
}
