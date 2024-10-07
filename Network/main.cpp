// #include "Servers/TestServer.hpp"

// int main()
// {
// 	TestServer server;
// 	return (0);
// }

#include "Servers/TCPServer.hpp"
#include <iostream>

int main() {
    try {
        // Create a TCPServer object with the following parameters:
        // domain: AF_INET (IPv4)
        // service: SOCK_STREAM (TCP)
        // protocol: 0 (default protocol for TCP)
        // port: 8080
        // interf: INADDR_ANY (bind to all available interfaces)
        // backlog: 10 (maximum number of pending connections in the queue)
        TCPServer tcpServer(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);

        // Run the server to start accepting connections
        tcpServer.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
