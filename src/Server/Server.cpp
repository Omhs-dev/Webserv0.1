// #include "Server.hpp"
// // Server.cpp
// #include <fstream>
// #include <sstream>
// #include <unistd.h>
// #include <iostream>

// Server::Server(int port, const std::vector<Location>& locations) : _locations(locations) {
//     _serverSocket.create();
//     _serverSocket.bind(port);
//     _serverSocket.listen(10);
//     _serverSocket.setNonBlocking();
    
//     pollfd serverPollFd;
//     serverPollFd.fd = _serverSocket.getSocketFd();
//     serverPollFd.events = POLLIN;
//     _pollfds.push_back(serverPollFd);
// }
#include "Server.hpp"
#include "Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

Server::Server(const ServerConfig& config) : serverConfigs(config) {
    // Create and configure the server socket
    _serverSocket.create();
    _serverSocket.bind(std::stoi(config._listen));
    _serverSocket.listen(10);
    _serverSocket.setNonBlocking();
	
	std::cout << "Server name: " << config._serverName << std::endl;
	std::cout << "Server listening on port: " << config._listen << std::endl;
	std::cout << "Server root directory: " << config._root << std::endl;
	std::cout << "Server index file: " << config._index << std::endl;

    // Add the server socket to the pollfd set
    pollfd serverPollFd;
    serverPollFd.fd = _serverSocket.getSocketFd();
    serverPollFd.events = POLLIN;  // We're interested in reading new connections
    pollfds.push_back(serverPollFd);
}

void Server::run() {
    while (true) {
        // Use poll() to handle multiple connections
        int activity = poll(pollfds.data(), pollfds.size(), -1);
        if (activity < 0) {
            std::cerr << "Error with poll" << std::endl;
            continue;
        }

        for (size_t i = 0; i < pollfds.size(); ++i) {
            if (pollfds[i].revents & POLLIN) {
                // Check if it's a new connection on the server socket
                if (pollfds[i].fd == _serverSocket.getSocketFd()) {
                    handleNewConnection();
                } else {
                    // Handle incoming data from existing clients
                    handleClient(pollfds[i].fd);
                }
            }
        }
    }
}

void Server::handleNewConnection() {
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(_serverSocket.getSocketFd(), (sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket < 0) {
        std::cerr << "Failed to accept client connection" << std::endl;
        return;
    }

    // Set client socket to non-blocking mode
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

    // Add the new client socket to the pollfd set
    pollfd clientPollFd;
    clientPollFd.fd = clientSocket;
    clientPollFd.events = POLLIN;  // We're interested in reading data from the client
    pollfds.push_back(clientPollFd);
}

void Server::handleClient(int client_fd) {
    Client client(client_fd);  // Create a client object to handle the connection
    client.handleRequest();    // Process the client's request

    // After handling the client, remove it from the pollfd set
    for (size_t i = 0; i < pollfds.size(); ++i) {
		if (pollfds[i].fd == client_fd) {
			pollfds.erase(pollfds.begin() + i);
			break;
		}
	}
}
