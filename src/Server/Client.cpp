#include "Client.hpp"
#include "../Response/HTTPResponse.hpp"

Client::Client(int socket, Server *server) : _server(server), _clientSocket(socket),  _request(new HTTPRequest(this)), _response(new HTTPResponse(this)) {}

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
}

void Client::handleRequest()
{
	char buffer[MAX_BUFFER_SIZE + 1];
	ssize_t bytesRead = recv(_clientSocket, buffer, MAX_BUFFER_SIZE, 0);
	if (bytesRead > 0)
	{
		buffer[bytesRead] = '\0'; // Null-terminate the buffer
		_request->parseRequest(std::string(buffer, bytesRead));
	
		Logger::Specifique(_request->getMethod(), "Method");
		Logger::Specifique(_request->getPath(), "Path");
		Logger::Specifique(_request->getVersion(), "Version");
	}
	else if (bytesRead == 0)
	{
		Logger::NormalCout("bytes is 0, message received...");
		throw ClientException();
	}
	else
	{
		Logger::ErrorCout("Error: bytes read");
		if (bytesRead == 1)
			Logger::SpecifiqueForInt(_clientSocket, "Failed to read from Fd");
	}
}

void Client::handleResponse()
{
	_response->generateResponse();
	sendResponse(_response->getData());
}

void Client::sendResponse(const std::string &response)
{
	ssize_t sent = send(_clientSocket, response.c_str(), response.size(), 0);
	// if(sent == -1)
	// {
	// 	Logger::ErrorCout("Failed to send");
	// 	return;
	// }
	// if (sent == 0)
	// {
	// 	throw ClientException();
	// }
	Logger::SpecifiqueForInt(sent, "Bytes sent");
}

//Getters

int Client::getClientSocket() const
{
	return _clientSocket;
}

Client::~Client()
{
	delete _request;
	delete _response;
	close(_clientSocket);
}
