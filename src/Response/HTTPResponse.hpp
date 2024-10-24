#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HTTPResponse {
public:
    HTTPResponse();
    static HTTPResponse fromFile(const std::string &filePath);
	
	void getIndexeFile();
    void setStatus(const std::string &statusCode, const std::string &statusMessage);
    void setHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &bodyContent);

    std::string getData() const;

private:
    std::string _statusCode;
    std::string _statusMessage;
    std::map<std::string, std::string> _headers;
    std::string _body;

    std::string getStatusLine() const;
    std::string getHeaders() const;
};

#endif
