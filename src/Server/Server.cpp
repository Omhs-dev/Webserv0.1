#include "Server.hpp"
#include "Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <csignal>

static volatile bool serverRunning = true;

void signalHandler(int signum) {
	if (signum == SIGINT)
	{
	    std::cout << "\nInterrupt signal (" << signum << ") received : ";
	    serverRunning = false;
	}
}

Server::Server(const HTTPConfigs &config) : _httpConfigs(config)
{
	// _state = SERVER_START;
	_serverRun = true;
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
	std::signal(SIGINT, signalHandler);
	while (serverRunning)
	{
		// Use poll() to handle multiple connections
		int activity = poll(pollfds.data(), pollfds.size(), -1);
		if (activity < 0)
		{
			// Logger::ErrorCout("Error in poll");
			break;
		}

		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			std::cout << "Server sockets: " << _serverSockets.size() << "; pollfds: " << pollfds.size() << std::endl;
			if (pollfds[i].revents & POLLIN)
			{
				    // if (i >= _serverSockets.size()) {
            		// 	std::cerr << "Error: pollfd index out of bounds!" << std::endl;
            		// 	continue;
        			// }
				// Check if it's a new connection on the server socket
				if (_serverSockets.size() > 0 && (pollfds[i].fd == _serverSockets[0] || pollfds[i].fd == _serverSockets[1]))
				{
					std::cout << "Handling new connection" << std::endl;
					// Logger::NormalCout("New connection on server socket");
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
	// Logger::NormalCout("Server stopped ! I am out of the loop");
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

	std::cout << "New client connected. fd: "<< clientSocket << std::endl;
}

void Server::handleClient(int client_fd)
{
	std::cout << "Handling client.." << std::endl;
	Client client(client_fd);		  // Create a client object to handle the connection
	client.clientConnectionProcess(); // Process the client's request
	// closeClient(client_fd);			  // Close the client connection
}

// After handling the client, remove it from the pollfd set
void Server::closeClient(int client_fd)
{
	close(client_fd);
	pollfds.erase(std::remove_if(pollfds.begin(), pollfds.end(), [client_fd](const pollfd &pfd)
								{ return pfd.fd == client_fd; }),pollfds.end());
	// Logger::NormalCout("cleanup and close sockets");
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

void Server::shutdown()
{
	Logger::NormalCout("Shutting down server...");
	// for (auto &clientSocket : _clientSockets)
	// {
		// Logger::NormalCout("Closing client socket...");
	// 	close(clientSocket);
	// }
	
	for (auto &serverSocket : _serverSockets)
	{
		// Logger::NormalCout("Closing server socket...");
		close(serverSocket);
		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			// Logger::NormalCout("Removing server socket from pollfds...");
			if (pollfds[i].fd == serverSocket)
			{
				pollfds.erase(pollfds.begin() + i);
				break;
			}
		}
	}
	
	// _clientSockets.clear();
	_serverSockets.clear();
	// Logger::NormalCout("Server shutdown complete");
}

// void Server::stop()
// {
// 	for (int serverSocket : _serverSockets)
// 	{
// 		close(serverSocket);
// 	}
// }

Server::~Server()
{
	// stop();
	shutdown();
}
