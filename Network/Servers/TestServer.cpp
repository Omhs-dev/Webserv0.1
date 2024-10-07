#include "TestServer.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

using namespace std;

// Constructor for TestServer with configuration from parsed file
TestServer::TestServer(HTTPConfigs &configs) 
    : Server(AF_INET, SOCK_STREAM, 0, std::stoi(configs._servers[0]._port), INADDR_ANY, 10), _httpConfigs(configs) 
{
    // Initialize client sockets to 0
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        _clientSockets[i] = 0;
    }

    // Initialize poll file descriptors
    initPoll();

    // Start the server
    run();
}

// Set socket to non-blocking mode
void TestServer::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

// Initialize poll descriptors for the server socket and clients
void TestServer::initPoll() {
    pollfd server_fd;
    server_fd.fd = get_socket()->get_socket();
    server_fd.events = POLLIN;  // Ready to accept connections
    _fds.push_back(server_fd);
}

// Accept incoming connections
void TestServer::accepter() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket = accept(get_socket()->get_socket(), (struct sockaddr*)&client_addr, &addr_len);

    if (new_socket < 0) {
        perror("accept");
        return;
    }

    setNonBlocking(new_socket);  // Set the new socket to non-blocking mode

    cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << endl;

    // Add the new socket to the poll file descriptors
    pollfd client_fd;
    client_fd.fd = new_socket;
    client_fd.events = POLLIN | POLLOUT;  // Ready for reading and writing
    _fds.push_back(client_fd);
}

// Override handler() from Server with no parameter (required to match the base class signature)
void TestServer::handler() {
    // This will be empty since the actual handling logic is in handleClient(int socket)
}

// Handle client data (socket passed as parameter)
void TestServer::handleClient(int socket) {
    memset(_buffer, 0, sizeof(_buffer));
    int bytes_received = read(socket, _buffer, sizeof(_buffer));

    if (bytes_received <= 0) {
        // Client disconnected or error occurred
        close(socket);
        cout << "Client disconnected" << endl;

        // Remove the socket from the poll descriptors
        for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].fd == socket) {
                _fds.erase(_fds.begin() + i);
                break;
            }
        }
    } else {
        // Print received message
        cout << "Received: " << _buffer << endl;
    }
}

// Override responder() from Server with no parameter (required to match the base class signature)
void TestServer::responder() {
    // This will be empty since the actual response logic is in respondToClient(int socket)
}

// Send response to client (with socket parameter)
void TestServer::respondToClient(int socket) {
    string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
    send(socket, response.c_str(), response.size(), 0);
}

// Main loop to run the server
void TestServer::run() {
    while (1) {
        cout << "---------------- Waiting for connections -----------------" << endl;

        // Poll for events (with a timeout of 1000 ms)
        int poll_count = poll(_fds.data(), _fds.size(), 1000);

        if (poll_count < 0) {
            perror("poll error");
            exit(EXIT_FAILURE);
        }

        // Loop through the poll file descriptors and check for events
        for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == get_socket()->get_socket()) {
                    // New incoming connection
                    accepter();
                } else {
                    // Data received from an existing client
                    handleClient(_fds[i].fd);
                    respondToClient(_fds[i].fd);
                }
            }
        }
    }
}
