// HTTPRequest.cpp
#include "../include/HTTPRequest.hpp"
#include <sstream>

HTTPRequest::HTTPRequest() : _complete(false) {}

void HTTPRequest::appendData(const char *data, size_t length) {
    _data.append(data, length);
    if (_data.find("\r\n\r\n") != std::string::npos) {
        _complete = true;
    }
}

bool HTTPRequest::isComplete() const {
    return _complete;
}

std::string HTTPRequest::getMethod() const {
    std::istringstream stream(_data);
    std::string method;
    stream >> method;
    return method;
}

std::string HTTPRequest::getPath() const {
    std::istringstream stream(_data);
    std::string method, path;
    stream >> method >> path;
    return path;
}
