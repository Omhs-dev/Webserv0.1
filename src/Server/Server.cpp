#include "Server.hpp"
#include "Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

Server::Server(const HTTPConfigs &config) : _httpConfigs(config)
{
	for (auto &serverConfig : config._servers)
	{
		_socketObject.create();
		_socketObject.bind(std::stoi(serverConfig._listen));
		_socketObject.listen(30);
		_socketObject.setNonBlocking();

		std::cout << "Server name: " << serverConfig._serverName << std::endl;
		std::cout << "Server listening on port: " << serverConfig._listen << std::endl;
		std::cout << "Server root directory: " << serverConfig._root << std::endl;
		std::cout << "Server index file: " << serverConfig._index << std::endl;

		// Add the server socket to the pollfd set
		pollfd serverPollFd;
		serverPollFd.fd = _socketObject.getSocketFd();
		serverPollFd.events = POLLIN; // We're interested in reading new connections
		pollfds.push_back(serverPollFd);

		_serverSockets.push_back(_socketObject.getSocketFd());
	}
	// Create and configure the server socket
}

void Server::run()
{
	while (true)
	{
		// Use poll() to handle multiple connections
		int activity = poll(pollfds.data(), pollfds.size(), -1);
		if (activity < 0)
		{
			std::cerr << "Error with poll" << std::endl;
			continue;
		}

		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			if (pollfds[i].revents & POLLIN)
			{
				// Check if it's a new connection on the server socket
				if (_serverSockets.size() > 0 && pollfds[i].fd == _serverSockets[i])
				{
					handleNewConnection(_serverSockets[i]);
				}
				else
				{
					// Otherwise, it's a client connection
					handleClient(pollfds[i].fd);
				}
			}
		}
	}
}

// Handle a new connection on the server socket
void Server::handleNewConnection(int server_fd)
{
	sockaddr_in clientAddr{};
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(server_fd, (struct sockaddr *)&clientAddr, &clientAddrLen);

	if (clientSocket < 0)
	{
		std::cerr << "Failed to accept client connection" << std::endl;
		return;
	}

	// Set client socket to non-blocking mode
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	// Add the new client socket to the pollfd set
	pollfd clientPollFd;
	clientPollFd.fd = clientSocket;
	clientPollFd.events = POLLIN; // We're interested in reading data from the client
	pollfds.push_back(clientPollFd);

	std::cout << "New client connected" << std::endl;
}

void Server::handleClient(int client_fd)
{
	Client client(client_fd);		  // Create a client object to handle the connection
	client.clientConnectionProcess(); // Process the client's request
	closeClient(client_fd);			  // Close the client connection
}

// After handling the client, remove it from the pollfd set
void Server::closeClient(int client_fd)
{
	close(client_fd);
	pollfds.erase(std::remove_if(pollfds.begin(), pollfds.end(), [client_fd](const pollfd &pfd)
								{ return pfd.fd == client_fd; }),pollfds.end());
}

// add a signal handler to close the server socket
// when the server is terminated or ctrl-c is pressed

// --- GETTERS ---

HTTPConfigs Server::getConfigs()
{
	return _httpConfigs;
}

Socket Server::getSocketObject()
{
	return _socketObject;
}

// --- DESTRUCTOR ---

void Server::stop()
{
	for (int serverSocket : _serverSockets)
	{
		close(serverSocket);
	}
}

Server::~Server()
{
	stop();
}
