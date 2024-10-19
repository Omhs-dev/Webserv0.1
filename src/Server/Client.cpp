#include "Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"

Client::Client(int socket) : clientSocket(socket) {}

void Client::clientConnectionProcess()
{
	handleRequest();
	handleResponse();
	close(clientSocket);
}

void Client::handleRequest()
{
	char buffer[MAX_BUFFER_SIZE + 1];
	int bytesRead = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);

		// std::cout << "Received " << bytesRead << " bytes from client" << std::endl;
	memset(buffer + bytesRead, 0, 1); // Null-terminate the buffer
	if (bytesRead > 0)
	{
		// Parse the HTTP request
		_request.parseRequest(std::string(buffer, bytesRead));

		// For debugging: print the parsed request method, path, and version
		std::cout << "Method: " << _request.getMethod() << std::endl;
		std::cout << "Path: " << _request.getPath() << std::endl;
		std::cout << "Version: " << _request.getVersion() << std::endl;

	}
	else if (bytesRead == 0)
	{
		throw ClientException();
	}
	else
	{
		std::cerr << "Error reading from client socket" << std::endl;
	}

	// close(clientSocket); // Close the connection after sending the response
}

// create a function handleResponse() that will handle the response
void Client::handleResponse()
{
	// Generate an appropriate HTTP response based on the request
		if (_request.getMethod() == "GET")
		{
			std::string filePath = _request.getPath(); // Example: map to server's file system

			// - this part is to be modified
			 // Serve index.html for root path "/"
            if (filePath == "/") {
                filePath = "/index.html";
            }

            // Map the path to the server's file system
            filePath = "./www" + filePath;  // Assuming the server's root directory is "./www"

            // Generate response from the file (this will serve the file if it exists)
            _response = HTTPResponse::fromFile(filePath);
		}
		else
		{
			// Handle other HTTP methods or errors (e.g., 405 Method Not Allowed)
			_response.setStatus("405", "Method Not Allowed");
		}

		// Send the response to the client
		sendResponse(_response.getData());
}

void Client::sendResponse(const std::string &response)
{
	// write(clientSocket, response.c_str(), response.size());
	send(clientSocket, response.c_str(), response.size(), 0);
}

Client::~Client()
{
	close(clientSocket);
}
