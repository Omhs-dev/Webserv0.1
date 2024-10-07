#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "Server.hpp"             // Base class
// #include "../Sockets/headerSocket.hpp"  // Includes necessary socket headers
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>                // For fcntl and non-blocking I/O
#include <map>                    // For managing multiple sockets
#include <arpa/inet.h>
#include <string>
#include <iostream>

#define MAX_CONNECTIONS 30

class TCPServer : public Server
{
    private:
        int clientSockets[MAX_CONNECTIONS];
        std::map<std::string, ListeningSocket *> serverSockets;
		// fd_set readfds;
        // void accepter();   // Accept new connections
        // void handler();    // Handle incoming data
        void  responder(int socketDescriptor, std::string port);  // Send response to client

        void setNonBlocking(int socket);
        void acceptConnection(std::string port);
        void initPoll(std::string port);
        void handleConnection(int socketDescriptor, std::string port);
        void handleWriting(int socketDescriptor);
        ListeningSocket *getServerSocket(std::string port);
        void removeSocket(int socket);

    public:
        TCPServer(int domain, int service, int protocol, int port, u_long interf, int bklog);
        void run();  // Override base class run method
};

#endif
