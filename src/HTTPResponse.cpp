#include "../include/HTTPResponse.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Default constructor (you can initialize default statusCode here)
HTTPResponse::HTTPResponse() : _statusCode(200) {  // Default to 200 OK
    _body = "<html><body><h1>200 OK</h1></body></html>";

    std::ostringstream headersStream;
    headersStream << "HTTP/1.1 " << _statusCode << " OK\r\n"
                  << "Content-Length: " << _body.size() << "\r\n"
                  << "Content-Type: text/html\r\n"
                  << "Connection: close\r\n\r\n";
    _headers = headersStream.str();
}

HTTPResponse::HTTPResponse(int statusCode) : _statusCode(statusCode) {
    if (statusCode == 405) {
        _body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
    }

    std::ostringstream headersStream;
    headersStream << "HTTP/1.1 " << statusCode << " OK\r\n"
                  << "Content-Length: " << _body.size() << "\r\n"
                  << "Content-Type: text/html\r\n"
                  << "Connection: close\r\n\r\n";
    _headers = headersStream.str();
}

HTTPResponse::HTTPResponse(int statusCode, const std::string &location) : _statusCode(statusCode) {
    _headers = "HTTP/1.1 301 Moved Permanently\r\n"
               "Location: " + location + "\r\n"
               "Connection: close\r\n\r\n";
}

HTTPResponse HTTPResponse::fromFile(const std::string &filePath) {
    HTTPResponse response;  // Now this will use the default constructor
    std::ifstream file(filePath);
    
    if (file.is_open()) {
        std::ostringstream bodyStream;
        bodyStream << file.rdbuf();
        response._body = bodyStream.str();
        response._statusCode = 200;  // OK
    } else {
        response._statusCode = 404;  // File not found
        response._body = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    std::ostringstream headersStream;
    headersStream << "HTTP/1.1 " << response._statusCode << " OK\r\n"
                  << "Content-Length: " << response._body.size() << "\r\n"
                  << "Content-Type: text/html\r\n"
                  << "Connection: close\r\n\r\n";
    
    response._headers = headersStream.str();
    return response;
}

std::string HTTPResponse::getData() const {
    return _headers + _body;
}
