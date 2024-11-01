#include "Client.hpp"
#include "../Response/HTTPResponse.hpp"

Client::Client(int socket) : _clientSocket(socket), _request(new HTTPRequest(this)) {}

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

void Client::handleResponse()
{
	HTTPResponse response;
	// checkLocationPath(_request->getPath());
	std::cout << "server root: " << _server->getConfigs()._servers[0].getRoot() << std::endl;
	std::cout << "loccation path: " << _server->getConfigs()._servers[0].getLocations()[0].getLocationPath() << std::endl;
	response.generateResponse(_request->getMethod(), _request->getPath());
	sendResponse(response.getData());
}

std::string Client::checkLocationPath(const std::string &path)
{
	std::cout << "Checking location path: " << path << std::endl;
	std::cout << "before for loop" << std::endl;
	for (auto &server : _server->getConfigs()._servers)
	{
		std::cout << "inside for loop" << std::endl;
		for (LocationConfig &location : server.getLocations())
		{
			std::cout << "inside for loop 2" << std::endl;
			if (path == location.getLocationPath())
			{ // Path matches location
				std::cout << "Location found: " << location.getLocationPath() << std::endl;
				// return location.getLocationPath(); // Return the matched location configuration
				// break;
			}
		}
		std::cout << "server location index: " << server.getIndex() << std::endl;
	}
	return "";
}

void Client::sendResponse(const std::string &response)
{
	send(_clientSocket, response.c_str(), response.size(), 0);
}

Client::~Client()
{
	delete _request;
	close(_clientSocket);
}
