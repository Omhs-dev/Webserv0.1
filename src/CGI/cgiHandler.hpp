#if !defined(CGI_HANDLER_HPP)
#define CGI_HANDLER_HPP

#include <unistd.h>    // For fork, execve
#include <sys/wait.h>  // For waitpid
#include <vector>      // For managing environment variables
#include <iostream>
#include <sstream>
#include <memory>

struct cgiResponse
{
    std::string headers;
    std::string body;
    int         status_code;
    std::string status;
};

std::unique_ptr<cgiResponse> handleCGIRequest(const HTTPRequest &httpRequest);
#endif // CGI_HANDLER_HPP
