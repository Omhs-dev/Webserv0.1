#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "Server.hpp"
#include "../Parse/ConfigUtils.hpp"
#include <unistd.h>
#include <cstring>
#include <string.h>
#include <fcntl.h>  // For non-blocking I/O
#include <poll.h>   // For poll()
#include <vector>

#define MAX_CONNECTIONS 1024  // Maximum number of concurrent connections

class TestServer: public Server
{
    private:
        char _buffer[30000];
        int _clientSockets[MAX_CONNECTIONS];   // Client socket descriptors
        std::vector<pollfd> _fds;              // File descriptors for poll
        HTTPConfigs _httpConfigs;              // Parsed configuration
        
        void setNonBlocking(int socket);       // Set socket to non-blocking mode
        void accepter();                       // Accept incoming connections
        void handler();                        // Override handler from base class (no parameter)
        void responder();                      // Override responder from base class (no parameter)
        void handleClient(int socket);         // Handle incoming data from a client (with parameter)
        void respondToClient(int socket);      // Send a response to the client (with parameter)
        void initPoll();                       // Initialize polling mechanism

    public:
        TestServer(HTTPConfigs &configs);      // Constructor with configuration
        void run();                            // Main loop
};

#endif
