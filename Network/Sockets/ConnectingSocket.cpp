#include "ConnectingSocket.hpp"

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interf)
				: Socket(domain, service, protocol, port, interf)
{
	set_connection(connect_network(get_socket(), get_address()));
	check_connection(get_connection());
}

int ConnectingSocket::connect_network(int sock, struct sockaddr_in address)
{
	return (connect(sock, (struct sockaddr *)&address, sizeof(address)));
}
