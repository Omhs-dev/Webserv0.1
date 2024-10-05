#include "Socket.hpp"

// Default constructor
Socket::Socket(int domain, int service, int protocol, int port, u_long interface)
{
	// define addresse structure
	_address.sin_family = domain;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(interface);
	// establish socket
	_sock = socket(domain, service, protocol);
	check_connection(_sock);
	// establish network connection
	// _connection = connect_network(_sock, _address);
	// check_connection(_connection);
}

// Test connection
void Socket::check_connection(int item)
{
	if (item < 0)
	{
		cerr << "Unable to connect..." << endl;
		exit(EXIT_FAILURE);
	}
}

// Getters
struct sockaddr_in Socket::get_address()
{
	return (_address);
}
int Socket::get_socket()
{
	return (_sock);
}
int Socket::get_connection()
{
	return (_connection);
}

void Socket::set_connection(int con)
{
	_connection = con;
}