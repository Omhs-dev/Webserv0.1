#include "HTTPResponse.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

HTTPResponse::HTTPResponse() : _statusCode("200"), _statusMessage("OK") {}

void HTTPResponse::generateResponse(const std::string &method, const std::string &path)
{
	if (method == "GET")
	{
		handleGet(path);
	}
}

void HTTPResponse::handleGet(const std::string &path)
{
	if (path == "/")
	{
		setDefaultResponse();
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

void HTTPResponse::setDefaultResponse()
{
	std::ifstream file("./www/index.html");

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		setBody(buffer.str());
		setStatus("200", "OK");
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

void HTTPResponse::setStatus(const std::string &code, const std::string &message)
{
	_statusCode = code;
	_statusMessage = message;
}

void HTTPResponse::setBody(const std::string &body)
{
	_body = body;
}

std::string HTTPResponse::getData() const
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << _body.size() << "\r\n";
	oss << "\r\n";
	oss << _body;
	return oss.str();
}
