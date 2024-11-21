#if !defined(CGI_HANDLER_HPP)
#define CGI_HANDLER_HPP

#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include <unistd.h>    // For fork, execve
#include <sys/wait.h>  // For waitpid
#include <vector>      // For managing environment variables
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>

class HTTPRequest;

struct cgiResponse
{
    std::string response_data;
    int         status_code;
    std::string status;

    //Constructor
    cgiResponse() :response_data(""), status_code(200), status("OK") {}
};

void handleCGIRequest(const HTTPRequest &httpRequest);
#endif // CGI_HANDLER_HPP
