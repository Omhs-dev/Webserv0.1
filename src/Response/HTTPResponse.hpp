#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>

class HTTPResponse {
public:
    HTTPResponse();
    static HTTPResponse fromFile(const std::string& filePath);
    std::string getData() const;
    void setStatus(const std::string& code, const std::string& message);

private:
    std::string statusCode;
    std::string statusMessage;
    std::string body;
    std::string headers;

    void buildResponse();
};

#endif
