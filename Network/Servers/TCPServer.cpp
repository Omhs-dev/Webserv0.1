#include "TCPServer.hpp"

// Constructor for TCPServer
TCPServer::TCPServer(int domain, int service, int protocol, int port, u_long interf, int bklog)
    : Server(domain, service, protocol, port, interf, bklog)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        clientSockets[i] = 0;
    }
}

// Set the socket to non-blocking mode
void TCPServer::setNonBlocking(int socket) {
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

// Correct function name and ensure the map contains the correct ListeningSocket
ListeningSocket *TCPServer::getServerSocket(std::string port) {
    return (serverSockets[port]);
}

void TCPServer::acceptConnection(std::string port) {
    struct sockaddr_in address = getServerSocket(port)->get_address();
    
    // We no longer use FD_ISSET with poll(). Just directly use poll() logic below.
    int addrlen = sizeof(address);
    int newSocket = accept(getServerSocket(port)->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    
    if (newSocket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    setNonBlocking(newSocket);

    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address.sin_addr), ip4, INET_ADDRSTRLEN);
    std::cout << "Connection accepted from " << ip4 << std::endl;

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clientSockets[i] == 0) {
            clientSockets[i] = newSocket;
            break;
        }
    }
}

// Initialize poll and handle polling events
void TCPServer::initPoll(std::string port) {
    struct pollfd fds[MAX_CONNECTIONS + 1];
    int nfds = 0;

    // Add the server socket for the given port
    fds[nfds].fd = getServerSocket(port)->get_socket();
    fds[nfds].events = POLLIN; // Check for incoming connections (POLLIN)
    nfds++;

    // Add client sockets to the pollfd array
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clientSockets[i] > 0) {
            fds[nfds].fd = clientSockets[i];
            fds[nfds].events = POLLIN | POLLOUT; // Check for readability (POLLIN) and writability (POLLOUT)
            nfds++;
        }
    }

    int pollResult = poll(fds, nfds, 2000); // 2 second timeout
    if (pollResult < 0) {
        perror("poll error");
        exit(EXIT_FAILURE);
    }

    // Handle the events for each file descriptor
    for (int i = 0; i < nfds; i++) {
        if (fds[i].revents & POLLIN) {
            if (fds[i].fd == getServerSocket(port)->get_socket()) {
                // Handle new incoming connection
                acceptConnection(port);
            } else {
                // Handle data from existing client socket
                handleConnection(fds[i].fd, port);
            }
        }

        if (fds[i].revents & POLLOUT) {
            // Handle client socket ready for writing
            handleWriting(fds[i].fd);
        }
    }
}

// Send response to the client
void TCPServer::responder(int socketDescriptor, std::string port) {
	(void)port;
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
    write(socketDescriptor, response.c_str(), response.length());
    std::cout << "Sent response to client on socket " << socketDescriptor << std::endl;

    close(socketDescriptor);
    removeSocket(socketDescriptor);
}

// Handle incoming connection data
void TCPServer::handleConnection(int socketDescriptor, std::string port) {
    char buffer[1024];
    int bytesRead = read(socketDescriptor, buffer, sizeof(buffer));

    if (bytesRead == 0) {
        std::cout << "Client disconnected\n";
        close(socketDescriptor);
        removeSocket(socketDescriptor);
    } else if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received request: " << buffer << std::endl;
        responder(socketDescriptor, port);
    }
}

// Handle data writing to the client
void TCPServer::handleWriting(int socketDescriptor) {
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
    write(socketDescriptor, response.c_str(), response.length());
    std::cout << "Sent response to client on socket " << socketDescriptor << std::endl;

    close(socketDescriptor);
    removeSocket(socketDescriptor);
}

// Remove a client socket from the clientSockets array
void TCPServer::removeSocket(int socket) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (clientSockets[i] == socket) {
            clientSockets[i] = 0;
            break;
        }
    }
}

// Main server loop to handle connections
void TCPServer::run() {
    while (1) {
        std::cout << "Waiting for connections...\n";
        for (std::map<std::string, ListeningSocket *>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it) {
            initPoll(it->first);
        }
    }
}
