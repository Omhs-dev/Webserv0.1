#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include "Server.hpp"

#define MAX_BUFFER_SIZE 1024

class HTTPRequest;
class HTTPResponse;
class Server;

class Client
{
	private:
	    Server *_server;
	    int _clientSocket;
		std::vector<ServerConfig>	_configs;
	    HTTPRequest *_request;
	    HTTPResponse *_response; // initialize this in constructor
	    void sendResponse(const std::string& response);

	public:
	    Client(int clientSocket, Server *server);
	    void clientConnectionProcess();
	    void handleRequest();
	    void handleResponse();
		
		std::string checkLocationPath(const std::string& path);
		
	    ~Client();
		
		Server *getServer() const { return _server; }
		HTTPRequest *getRequest() const { return _request; }
		int			getClientSocket() const;
		std::vector<ServerConfig> getConfigs() const;
		
		class ClientException : public std::exception
		{
			public:
			    virtual const char* what() const throw()
			    {
			        return "";
			        // return "Client is disconnected";
			    }
		};
};

#endif
