// HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
#include <string>

class HTTPResponse {
public:
	HTTPResponse();
    static HTTPResponse fromFile(const std::string &filePath);
    HTTPResponse(int statusCode);
    HTTPResponse(int statusCode, const std::string &location); // For redirection
    std::string getData() const;

private:
    int _statusCode;
    std::string _body;
    std::string _headers;
};

#endif
