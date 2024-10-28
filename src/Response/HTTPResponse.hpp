// HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>

#include "../Request/HTTPRequest.hpp"
#include "../Server/Client.hpp"

#define MAX_ALLOWED_FILE_SIZE 1024 * 1024
#define MAX_RESPONSE_BODY_SIZE 1024 * 1024

class HTTPRequest;
class Client;

// Set a content type based on the file extension
// set an error page message

// Generate reponse
// handle methodes
// handle GET
// check if the path is /
// if yes serve by default index.html

#include <string>

class HTTPResponse
{
private:
	void handleGet(const std::string &path);
	void setDefaultResponse(); // Serves default index content
	void setStatus(const std::string &code, const std::string &message);
	void setBody(const std::string &body);

	std::string _statusCode;
	std::string _statusMessage;
	std::string _headers;
	std::string _body;

public:
	HTTPResponse();
	void generateResponse(const std::string &method, const std::string &path);
	std::string getData() const;
};

#endif
