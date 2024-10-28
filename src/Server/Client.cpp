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
	int bytesRead = recv(_clientSocket, buffer, MAX_BUFFER_SIZE, 0);
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
	// Check if the request is for a specific file path
	// std::string filePath = "./www" + _request->getPath();
	
	// Generate response from file if it exists, otherwise default response
	// if (_request->getMethod() == "GET") {
	//     *_response = HTTPResponse::fromFile(filePath);  // Create response from file
	//     if (_response->getData().empty()) {  // If no content found, set default response
	//         _response->setStatus("200", "OK");
	//         _response->setBody("<html><body><h1>Hello World</h1></body></html>");
	//     }
	// } else {
	//     // For non-GET methods, set a simple 405 response
	//     _response->setStatus("405", "Method Not Allowed");
	//     _response->setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
	// }

	// Send the response to the client
	std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 20\r\n" // Length of the HTML content
        "\r\n"
        "<h1>Hello World</h1>";

	sendResponse(_response->getData());
	// sendResponse(_response->getData());
	std::cout << "connect to data : " << _response->getData() << std::endl;
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
