#include "HTTPResponse.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

HTTPResponse::HTTPResponse() : _statusCode("200"), _statusMessage("OK") {
    _headers["Content-Type"] = "text/html";  // Default content type
}

HTTPResponse HTTPResponse::fromFile(const std::string &filePath) {
    HTTPResponse response;
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();  // Read the file contents into a string
        response.setBody(buffer.str());

        // Set the content type based on the file extension
        if (filePath.find(".html") != std::string::npos) {
            response.setHeader("Content-Type", "text/html");
        } else if (filePath.find(".css") != std::string::npos) {
            response.setHeader("Content-Type", "text/css");
        } else if (filePath.find(".js") != std::string::npos) {
            response.setHeader("Content-Type", "application/javascript");
        } else {
            response.setHeader("Content-Type", "text/plain");
        }

        // Set content length
        response.setHeader("Content-Length", std::to_string(response._body.size()));
    } else {
        // File not found: return a 404 error
        response.setStatus("404", "Not Found");
        response.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", std::to_string(response._body.size()));
    }

    return response;
}

void HTTPResponse::setStatus(const std::string &statusCode, const std::string &statusMessage) {
    _statusCode = statusCode;
    _statusMessage = statusMessage;
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value) {
    _headers[key] = value;
}

void HTTPResponse::setBody(const std::string &bodyContent) {
    _body = bodyContent;
}

std::string HTTPResponse::getStatusLine() const {
    return "HTTP/1.1 " + _statusCode + " " + _statusMessage + "\r\n";
}

std::string HTTPResponse::getHeaders() const {
    std::string headersStr;
    for (const auto &header : _headers) {
        headersStr += header.first + ": " + header.second + "\r\n";
    }
    return headersStr;
}

std::string HTTPResponse::getData() const {
    return getStatusLine() + getHeaders() + "\r\n" + _body;
}
