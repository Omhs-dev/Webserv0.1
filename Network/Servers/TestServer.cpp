#include "TestServer.hpp"

TestServer::TestServer()
	: Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
	run();
}

void TestServer::accepter()
{
	struct sockaddr_in address = get_socket()->get_address();
	int addrlen = sizeof(address);
	_new_socket = accept(get_socket()->get_socket(), (struct sockaddr *)&address, (socklen_t*)&addrlen);
	read(_new_socket, _buffer, 30000);
}

void TestServer::handler()
{
	cout << "Message received: " << _buffer << endl;
}

void TestServer::responder()
{
	// send(_new_socket, "Message received", strlen("Message received"), 0);
	char *message = "Message received";
	write(_new_socket, message, strlen(message));
	close(_new_socket);
}

void TestServer::run()
{
	while (1)
	{
		cout << "-----------------Waiting for connection---------------" << endl;
		accepter();
		handler();
		responder();
		cout << "-----------------Connection closed---------------------" << endl;
	}
}