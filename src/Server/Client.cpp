#include "Client.hpp"

Client::Client(int socket) : clientSocket(socket)
{
	_request = nullptr;
}

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
		_request = new HTTPRequest(this);
		// Parse the HTTP request
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

	// close(clientSocket); // Close the connection after sending the response
}


// create a function handleResponse() that will handle the response
void Client::handleResponse()
{
    if (_request->getMethod() == "GET")
    {
        std::string filePath = _request->getPath();

        // Serve index.html for the root path "/"
        if (filePath == "/")
        {
            filePath = "/index.html";  // Default to index.html if the path is "/"
        }

        // Assuming the server root directory is configured and stored in the server config
        std::string rootDirectory = _request->getServer()->_root;  // Root directory from server config

        // Map the path to the server's file system using the root directory
        filePath = rootDirectory + filePath;

        // Check if the file exists (simple file existence check, could be expanded)
        std::ifstream file(filePath);
        if (!file.good())
        {
            std::cerr << "File not found: " << filePath << std::endl;
            _response = new HTTPResponse();
            _response->setStatus("404", "Not Found");
        }
        else
        {
            // Generate response from the file (serve the file if it exists)
            _response = new HTTPResponse(HTTPResponse::fromFile(filePath));
        }
    }
    else
    {
        // Handle other HTTP methods or errors (e.g., 405 Method Not Allowed)
        _response = new HTTPResponse();
        _response->setStatus("405", "Method Not Allowed");
    }

    // Send the response to the client
    sendResponse(_response->getData());
}




void Client::sendResponse(const std::string &response)
{
	// write(clientSocket, response.c_str(), response.size());
	send(clientSocket, response.c_str(), response.size(), 0);
}

Client::~Client()
{
    close(clientSocket);
    
    // // Clean up dynamically allocated memory
    // if (_request) {
    // std::cout << "Deleting request object" << std::endl;
    // delete _request;
    // _request = nullptr;
	// }
	// if (_response) {
	//     std::cout << "Deleting response object" << std::endl;
	//     delete _response;
	//     _response = nullptr;
	// }
}
