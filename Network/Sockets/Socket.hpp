#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

using namespace std;
/*
	Create a socket
	Bind the socket to an address IP and port
	Listen for incoming connections
	Accept incoming connections
	Close the listening socket
	While receiving, display the message
	close socket
*/
class Socket
{
	private:
		int _sock;
		int _connection;
		struct sockaddr_in _address;

	public:
		Socket(int domain, int service, int protocol, int port, u_long interface);
		virtual int connect_network(int sock, struct sockaddr_in address) = 0;
		
		void check_connection(int);
		
		struct sockaddr_in get_address();
		int get_socket();
		int get_connection();
		void set_connection(int con);
};
