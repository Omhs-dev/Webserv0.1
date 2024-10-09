// Socket.cpp
#include "../include/Socket.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

Socket::Socket() : _sockfd(-1) {}

Socket::~Socket() {
    if (_sockfd != -1) {
        close(_sockfd);
    }
}

void Socket::create() {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        handleError("Failed to create socket");
    }
}

void Socket::bind(int port) {
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(port);

    if (::bind(_sockfd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        handleError("Failed to bind socket");
    }
}

void Socket::listen(int backlog) {
    if (::listen(_sockfd, backlog) < 0) {
        handleError("Failed to listen on socket");
    }
}

void Socket::setNonBlocking() {
    int flags = fcntl(_sockfd, F_GETFL, 0);
    if (flags == -1) {
        handleError("Failed to get socket flags");
    }

    if (fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        handleError("Failed to set socket to non-blocking");
    }
}

int Socket::acceptConnection() {
    int clientSocket = accept(_sockfd, nullptr, nullptr);
    if (clientSocket < 0) {
        handleError("Failed to accept connection");
    }
    return clientSocket;
}

int Socket::getSocketFd() const {
    return _sockfd;
}

void Socket::handleError(const std::string &message) {
    std::cerr << message << ": " << strerror(errno) << std::endl;
    throw std::runtime_error(message);
}
