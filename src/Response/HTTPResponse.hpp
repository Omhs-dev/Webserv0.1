// HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>

class HTTPResponse {
private:
    std::string _statusCode;
    std::string _statusMessage;
    std::string _headers;
    std::string _body;

public:
    HTTPResponse();

    // Generate response based on file path
    static HTTPResponse fromFile(const std::string& filePath);

    // Generate response based on method and file path
    static HTTPResponse generateResponse(const std::string& method, const std::string& filePath);

    // Setters
    void setStatus(const std::string& code, const std::string& message);
    void setBody(const std::string& body);
	void getBody();
	void setHeader(const std::string &key, const std::string &value);
    
    
    // Get formatted response data for sending
    std::string getData() const;

    // Helper method for common headers
    void setDefaultHeaders();
};

#endif
