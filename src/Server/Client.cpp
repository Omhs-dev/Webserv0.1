#include "Client.hpp"
#include "../Response/HTTPResponse.hpp"

Client::Client(int socket, std::vector<ServerConfig> &configs) : _clientSocket(socket),  _configs(configs),  _request(new HTTPRequest(this)), _response(new HTTPResponse(this)) {}

void Client::clientConnectionProcess()
{
	std::cout << "Processing client (fd: " << _clientSocket << ") request..\n";
	try
	{
		handleRequest();
		handleResponse();
	}
	catch (const ClientException &e)
	{
		std::cerr << e.what() << std::endl;
	}
	// close(_clientSocket);
}

void Client::handleRequest()
{
	std::cout << "Handling request..\n";
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
		std::cout << "Error: bytes read: " << bytesRead << std::endl;
}

void Client::handleResponse()
{
	// HTTPResponse response;
	// checkLocationPath(_request->getPath());
	// std::cout << "server root: " << _server->getConfigs()._servers[0].getRoot() << std::endl;
	// std::cout << "location path: " << _server->getConfigs()._servers[0].getLocations()[0].getLocationPath() << std::endl;
	std::cout << "Handling response..\n";
	_response->generateResponse();
	sendResponse(_response->getData());
	std::cout << "CHECK\n";
}

std::string Client::checkLocationPath(const std::string &path)
{
	std::cout << "Checking location path: " << path << std::endl;
	std::cout << "before for loop" << std::endl;
	for (auto &server : _server->getConfigs())
	{
		std::cout << "inside for loop 2" << std::endl;
		for (LocationConfig &location : server.getLocations())
		{
			std::cout << "inside for loop 2" << std::endl;
			if (path == location.getLocationPath() || path.find(location.getLocationPath()) == 0)
			{ // Path matches location
				std::cout << "Location found: " << location.getLocationPath() << std::endl;
				return location.getLocationPath(); // Return the matched location configuration
				break;
			}
		}
		std::cout << "server location index: " << server.getIndex() << std::endl;
	}
	return "";
}

void Client::sendResponse(const std::string &response)
{
	std::cout << "Sending response to fd:" << _clientSocket <<"\n";
	ssize_t sent = send(_clientSocket, response.c_str(), response.size(), 0);
	std::cout << sent;
}

//Getters
std::vector<ServerConfig> Client::getConfigs() const
{
	return _configs;
}

Client::~Client()
{
	delete _request;
	// delete _response;
	close(_clientSocket);
}
