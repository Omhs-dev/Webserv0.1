#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string.h>
#include "HTTPRequest.hpp"
#include <fstream>
#include <sys/stat.h>
#include <sys/poll.h>
// #include "Client.hpp"
// #include "CgiHandler.hpp"


class HTTPRequest;

class HTTPResponse {
public:

    enum e_response_state
    {
        INIT,
        PROCESS,
        CHUNK,
        FINISH
    };
    
    HTTPResponse();
    static HTTPResponse fromFile(const std::string& filePath);
    std::string getData() const;
    void setStatus(const std::string& code, const std::string& message);

private:
    HTTPRequest* HTTPRequest;
    std::string HTTPResponse;

    int     statusCode;
    std::string statusMessage;
    std::string body;
    std::string headers;

    void buildResponse();

    //method 
    void handlePostRequest(void);
    void handleGetRequest(void);
    void handleDeleteRequest(void);
    int generateResponse(int pollFD);


};

#endif

