#include "HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest() {}

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
    std::istringstream stream(line);
    stream >> method >> path >> version;  // Extract method, path, and version
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

std::string HTTPRequest::getMethod() const {
    return method;
}

std::string HTTPRequest::getPath() const {
    return path;
}

std::string HTTPRequest::getVersion() const {
    return version;
}
