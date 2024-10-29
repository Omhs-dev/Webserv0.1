#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <iostream>

// #include "../header.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include "Server.hpp"

#define MAX_BUFFER_SIZE 1024

class HTTPRequest;
class HTTPResponse;

class Client
{
	private:
	    int _clientSocket;
	    Server *_server;
	    HTTPRequest *_request;
	    HTTPResponse *_response;
	    void sendResponse(const std::string& response);

	public:
	    Client(int clientSocket);
	    void clientConnectionProcess();
	    void handleRequest();
	    void handleResponse();
	    ~Client();
		
		Server *getServer() const { return _server; }
		
		class ClientException : public std::exception
		{
			public:
			    virtual const char* what() const throw()
			    {
			        return "Client is disconnected";
			    }
		};
};

#endif
