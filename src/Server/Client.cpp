#include "Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include <unistd.h>
#include <iostream>

Client::Client(int socket) : clientSocket(socket) {}

void Client::handleRequest() {
    char buffer[1024];
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));

    if (bytesRead > 0) {
        // Parse the HTTP request
        HTTPRequest request;
        request.parseRequest(std::string(buffer, bytesRead));

        // For debugging: print the parsed request method, path, and version
        std::cout << "Method: " << request.getMethod() << std::endl;
        std::cout << "Path: " << request.getPath() << std::endl;
        std::cout << "Version: " << request.getVersion() << std::endl;

        // Generate an appropriate HTTP response based on the request
        HTTPResponse response;
        if (request.getMethod() == "GET") {
            std::string filePath = request.getPath();  // Example: map to server's file system
            
            // - this part is to be modified
            if (filePath == "/" || filePath.back() == '/') {
				filePath += "index.html";  // Example: map / to index.html
			}
            
            filePath = "./www" + filePath;  // Example: prepend www root directory
            // -
            
            response = HTTPResponse::fromFile(filePath);  // Generate response from the file
        } else {
            // Handle other HTTP methods or errors (e.g., 405 Method Not Allowed)
            response.setStatus("405", "Method Not Allowed");
        }

        // Send the response to the client
        sendResponse(response.getData());
    } else {
        std::cerr << "Error reading from client socket" << std::endl;
    }

    close(clientSocket);  // Close the connection after sending the response
}

// create a function handleResponse() that will handle the response

void Client::sendResponse(const std::string& response) {
    write(clientSocket, response.c_str(), response.size());
}

Client::~Client() {
    close(clientSocket);
}
