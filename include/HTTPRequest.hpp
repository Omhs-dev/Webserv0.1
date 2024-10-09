// HTTPRequest.hpp
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

class HTTPRequest {
public:
    HTTPRequest();
    void appendData(const char *data, size_t length);
    bool isComplete() const;
    std::string getMethod() const;
    std::string getPath() const;

private:
    std::string _data;
    bool _complete;
};

#endif
