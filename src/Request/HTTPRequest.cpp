#include "HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest()
{
	method = "";
	path = "";
	version = "";
}

void HTTPRequest::parseRequest(const std::string& requestData) {
    std::istringstream stream(requestData);
    std::string line;
    bool isFirstLine = true;

    // Parse the request line and headers
    while (std::getline(stream, line) && !line.empty()) {
        if (isFirstLine) {
            parseRequestLine(line);  // Parse the first line (request line)
            isFirstLine = false;
        } else {
            parseHeaderLine(line);  // Parse headers
        }
    }
}

void HTTPRequest::parseRequestLine(const std::string& line) {
    method = parseMethod(line);
    path = parsePath(line);
    version = parseVersion(line);
    std::cout << "Parsed Request Line: " << method << " " << path << " " << version << std::endl;
}

void HTTPRequest::parseHeaderLine(const std::string& line) {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        headers[key] = value;
        std::cout << "Parsed Header: " << key << ": " << value << std::endl;
    }
}

std::string HTTPRequest::parseMethod(const std::string& line)
{
	// Find the position of the first space
    size_t firstSpace = line.find(' ');
    
    // If no space is found, it's an invalid request
    if (firstSpace == std::string::npos) {
        throw std::invalid_argument("Invalid Request Line: Method not found");
    }

    // Return the substring from the start of the line to the first space
    return line.substr(0, firstSpace);
}
std::string HTTPRequest::parsePath(const std::string& line) {
    // Find the first and second spaces
    size_t firstSpace = line.find(' ');
    size_t secondSpace = line.find(' ', firstSpace + 1);

    // If no second space is found, it's an invalid request
    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        throw std::invalid_argument("Invalid Request Line: Path not found");
    }

    // Return the substring between the first and second spaces
    return line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

std::string HTTPRequest::parseVersion(const std::string& line) {
    // Find the second space
    size_t secondSpace = line.find(' ', line.find(' ') + 1);

    // If no second space is found, it's an invalid request
    if (secondSpace == std::string::npos) {
        throw std::invalid_argument("Invalid Request Line: Version not found");
    }

    // Return the substring from the second space to the end of the line
    return line.substr(secondSpace + 1);
}


std::string HTTPRequest::getMethod() const {
    return method;
}

std::string HTTPRequest::getPath() const {
    return path;
}

std::string HTTPRequest::getVersion() const {
    return version;
}
