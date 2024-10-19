#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>
#include <iostream>

#include "../header.hpp"

#define MAX_BUFFER_SIZE 1024

class Client
{
	private:
	    int clientSocket;
	    HTTPRequest _request;
	    HTTPResponse _response;
	    void sendResponse(const std::string& response);

	public:
	    Client(int clientSocket);
	    void clientConnectionProcess();
	    void handleRequest();
	    void handleResponse();
	    ~Client();

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
