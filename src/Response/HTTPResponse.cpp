#include "HTTPResponse.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

HTTPResponse::HTTPResponse() {}

void HTTPResponse::setStatus(const std::string& code, const std::string& message) {
    statusCode = code;
    statusMessage = message;
}

HTTPResponse HTTPResponse::fromFile(const std::string& filePath) {
    HTTPResponse response;
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::ostringstream bodyStream;
        bodyStream << file.rdbuf();
        response.body = bodyStream.str();
        response.setStatus("200", "OK");
    } else {
        response.setStatus("404", "Not Found");
        response.body = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    response.buildResponse();
    return response;
}

void HTTPResponse::buildResponse() {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    responseStream << "Content-Length: " << body.size() << "\r\n";
    responseStream << "Content-Type: text/html\r\n";
    responseStream << "\r\n";
    responseStream << body;

    headers = responseStream.str();
}

std::string HTTPResponse::getData() const {
    return headers + body;
}
