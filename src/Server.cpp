// Server.cpp
#include "../include/Server.hpp"
// Server.cpp
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>

Server::Server(int port, const std::vector<Location>& locations) : _locations(locations) {
    _serverSocket.create();
    _serverSocket.bind(port);
    _serverSocket.listen(10);
    _serverSocket.setNonBlocking();
    
    pollfd serverPollFd;
    serverPollFd.fd = _serverSocket.getSocketFd();
    serverPollFd.events = POLLIN;
    _pollfds.push_back(serverPollFd);
}

Server::~Server() {}

void Server::run() {
    while (true) {
        int activity = poll(_pollfds.data(), _pollfds.size(), -1);
        if (activity > 0) {
            for (auto &pfd : _pollfds) {
                if (pfd.revents & POLLIN) {
                    if (pfd.fd == _serverSocket.getSocketFd()) {
                        handleNewConnection();
                    } else {
                        handleClient(pfd.fd);
                    }
                }
            }
        }
    }
}

void Server::handleNewConnection() {
    int clientSocket = _serverSocket.acceptConnection();
    
    pollfd clientPollFd;
    clientPollFd.fd = clientSocket;
    clientPollFd.events = POLLIN;
    _pollfds.push_back(clientPollFd);
    _requests[clientSocket] = HTTPRequest();
}


void Server::handleClient(int client_fd) {
    char buffer[1024];
    ssize_t bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        _requests[client_fd].appendData(buffer, bytesRead);
        if (_requests[client_fd].isComplete()) {
            std::string requestPath = _requests[client_fd].getPath();
            Location matchedLocation = matchLocation(requestPath);

            // Check if the requested path is a directory
            std::string filePath = matchedLocation.root + requestPath;

            struct stat pathStat;
            if (stat(filePath.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
                // If it's a directory, append "index.html" to the file path
                if (filePath.back() != '/') {
                    filePath += "/";
                }
                filePath += "index.html";  // Serve index.html by default in directories
            }

            HTTPResponse response = HTTPResponse::fromFile(filePath);
            send(client_fd, response.getData().c_str(), response.getData().size(), 0);
            closeClient(client_fd);
        }
    } else if (bytesRead == 0) {
        closeClient(client_fd);
    }
}

void Server::closeClient(int client_fd) {
    close(client_fd);
    _pollfds.erase(std::remove_if(_pollfds.begin(), _pollfds.end(), [client_fd](pollfd &pfd) {
        return pfd.fd == client_fd;
    }), _pollfds.end());
    _requests.erase(client_fd);
}

Location Server::matchLocation(const std::string &requestPath) {
    for (const auto &location : _locations) {
        if (requestPath.find(location.path) == 0) {
            return location;
        }
    }
    return _locations[0]; // Default to root location
}

bool Server::isMethodAllowed(const Location &location, const std::string &method) {
    return std::find(location.methods.begin(), location.methods.end(), method) != location.methods.end();
}
