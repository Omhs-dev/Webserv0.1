#include "Client.hpp"
#include <cstring>
#include <iostream>

Client::Client(int socket) : _clientSocket(socket), _request(new HTTPRequest(this)), _response(new HTTPResponse()) {}

void Client::clientConnectionProcess()
{
	try
	{
		handleRequest();
		handleResponse();
	}
	catch (const ClientException &e)
	{
		std::cerr << e.what() << std::endl;
	}
	close(_clientSocket);
}

void Client::handleRequest()
{
	char buffer[MAX_BUFFER_SIZE + 1];
	int bytesRead = 0;
	bytesRead = recv(_clientSocket, buffer, MAX_BUFFER_SIZE, 0);
	if (bytesRead > 0)
	{
		buffer[bytesRead] = '\0'; // Null-terminate the buffer
		_request->parseRequest(std::string(buffer, bytesRead));

		// For debugging: print the parsed request method, path, and version
		std::cout << "Method: " << _request->getMethod() << std::endl;
		std::cout << "Path: " << _request->getPath() << std::endl;
		std::cout << "Version: " << _request->getVersion() << std::endl;
	}
	else if (bytesRead == 0)
	{
		throw ClientException();
	}
	else
	{
		std::cerr << "Error reading from client socket" << std::endl;
	}
}

void Client::handleResponse() {
    std::string method = _request->getMethod();
    std::string filePath = _request->getPath();

    // Check if request method is GET
    // if (method == "GET") {
    //     // First, check if the path matches any configured location
    //     if (_request->getLocation()) {
    //         LocationConfig* location = _request->getLocation();
            
    //         // If the location has a specified index file, use it
    //         filePath = location->getRoot() + (filePath == "/" ? location->getIndex() : filePath);
    //     } 
    //     else if (_request->getServer()) {
    //         // Otherwise, check the server configuration for an index file
    //         ServerConfig* server = _request->getServer();
    //         filePath = server->getRoot() + (filePath == "/" ? server->getIndex() : filePath);
    //     } 
    //     else {
    //         // Default to "index.html" in the root directory if no location or server-specific index is found
    //         filePath += "index.html";
    //     }
    // }

    // Generate response from the file
    *_response = HTTPResponse::generateResponse(method, filePath);
	
    // Send the response to the client
    sendResponse(_response->getData());
}

void Client::sendResponse(const std::string &response)
{
	send(_clientSocket, response.c_str(), response.size(), 0);
}

Client::~Client()
{
	delete _request;
	delete _response;
	close(_clientSocket);
}
