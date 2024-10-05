#include "TestServer.hpp"

// Constructor for the TestServer class
TestServer::TestServer()
            : Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
    run();
}

// Accepts incoming client connections
void TestServer::accepter()
{
    // Get the socket's address structure (IP and port details)
    struct sockaddr_in address = get_socket()->get_address();
    int addrlen = sizeof(address);
    // Accept a new connection and store the client's socket descriptor in _new_socket
    _new_socket = accept(get_socket()->get_socket(), (struct sockaddr *)&address, (socklen_t*)&addrlen);
    // Read the incoming data from the client and store it in the buffer (_buffer)
    read(_new_socket, _buffer, 30000);  // Max size of the buffer is 30,000 bytes
}

// Handles the client's request (here, it simply prints the message)
void TestServer::handler()
{
    cout << "Message received: " << _buffer << endl;
}

// Sends a response back to the client
void TestServer::responder()
{
    send(_new_socket, "Message received", strlen("Message received"), 0);
    // char *message = "Message received";
    // write(_new_socket, message, strlen(message));
    // close(_new_socket);
}

// Main server loop to keep accepting, handling, and responding to client connections
void TestServer::run()
{
    while (1)
    {
        cout << "-----------------Waiting for connection---------------" << endl;
        accepter();
        handler();
        responder();
        cout << "-----------------Connection closed---------------------" << endl;
        cout << endl << endl;
    }
}
