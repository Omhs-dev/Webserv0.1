#include "HTTPRequest.hpp"
#include "../Server/Client.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest()
{
	_method = "";
	_uriPath = "";
	_version = "";
}

void HTTPRequest::parseRequest(const std::string &requestData)
{
	std::istringstream stream(requestData);
	std::string line;
	bool isFirstLine = true;

	// Parse the request line and headers
	while (std::getline(stream, line) && !line.empty())
	{
		// std::cout << "Parsing full Line: " << line << std::endl;
		if (isFirstLine)
		{
			// std::cout << "Parsing Request Line " << line << std::endl;
			parseRequestLine(line); // Parse the first line (request line)
			isFirstLine = false;
		}
		else
		{
			parseHeaderLine(line); // Parse headers
		}
	}
}

void HTTPRequest::parseRequestLine(const std::string &line)
{
	_method = parseMethod(line);
	_uriPath = parsePath(line);
	_version = parseVersion(line);
	std::cout << "Parsed Request Line: " << _method << " " << _uriPath << " " << _version << std::endl;
}

// void HTTPRequest::parseHeaderLine(const std::string& line) {
//     _headerKey = parseHeaderKey(line);
//     _headerValue = parseHeaderValue(line);
//     // _headers[_headerKey] = _headerValue;
//     std::cout << "Parsed Header: " << _headerKey << ": " << _headerValue << std::endl;
// }

void HTTPRequest::parseHeaderLine(const std::string &line)
{
	size_t colonPos = line.find(':');
	if (colonPos != std::string::npos)
	{
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		_headers[key] = value;
		std::cout << "Parsed Header: " << key << ": " << value << std::endl;
	}
}

std::string HTTPRequest::parseMethod(const std::string &line)
{
	// Find the position of the first space
	size_t firstSpace = line.find(' ');

	// If no space is found, it's an invalid request
	if (firstSpace == std::string::npos)
	{
		throw std::invalid_argument("Invalid Request Line: Method not found");
	}

	// Return the substring from the start of the line to the first space
	return line.substr(0, firstSpace);
}

std::string HTTPRequest::parsePath(const std::string &line)
{
	// Find the first and second spaces
	size_t firstSpace = line.find(' ');
	size_t secondSpace = line.find(' ', firstSpace + 1);
	
	std::cout << "First Space: " << firstSpace << std::endl;
	std::cout << "Second Space: " << secondSpace << std::endl;
	std::cout << "Line: " << line << std::endl;
	// If no second space is found, it's an invalid request
	if (firstSpace == std::string::npos || secondSpace == std::string::npos)
	{
		throw std::invalid_argument("Invalid Request Line: Path not found");
	}

	// Return the substring between the first and second spaces
	return line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

std::string HTTPRequest::parseVersion(const std::string &line)
{
	// Find the second space
	size_t secondSpace = line.find(' ', line.find(' ') + 1);

	// If no second space is found, it's an invalid request
	if (secondSpace == std::string::npos)
	{
		throw std::invalid_argument("Invalid Request Line: Version not found");
	}

	// Return the substring from the second space to the end of the line
	return line.substr(secondSpace + 1);
}

std::string HTTPRequest::parseHeaderKey(const std::string &line)
{
	// Find the position of the first colon
	size_t colonPos = line.find(':');

	// If no colon is found, it's an invalid header
	if (colonPos == std::string::npos)
	{
		throw std::invalid_argument("Invalid Header: Key not found");
	}

	// Return the substring from the start of the line to the colon
	return line.substr(0, colonPos);
}
std::string HTTPRequest::parseHeaderValue(const std::string &line)
{
	// Find the position of the first colon
	size_t colonPos = line.find(':');

	// If no colon is found, it's an invalid header
	if (colonPos == std::string::npos)
	{
		throw std::invalid_argument("Invalid Header: Value not found");
	}

	// Return the substring from the colon to the end of the line
	return line.substr(colonPos + 1);
}

// ------------------- GETTERS ----------------------

Client *HTTPRequest::getClient() const
{
	return _client;
}

LocationConfig *HTTPRequest::getLocation() const
{
	return _location;
}

ServerConfig *HTTPRequest::getServer() const
{
	return _server;
}

std::string HTTPRequest::getMethod() const
{
	return _method;
}

std::string HTTPRequest::getPath() const
{
	return _uriPath;
}

std::string HTTPRequest::getVersion() const
{
	return _version;
}

std::map<std::string, std::string> HTTPRequest::getHeaders() const
{
	return _headers;
}

std::string HTTPRequest::getHeaderKey() const
{
	return _headerKey;
}

std::string HTTPRequest::getHeaderValue() const
{
	return _headerValue;
}