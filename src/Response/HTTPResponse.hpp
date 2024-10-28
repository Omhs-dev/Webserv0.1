#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <fstream>
#include <sstream>

// function that generates responses based on the req
   // function to handle delete req
   // function to handle put req
   // function to handle get req
   // for the function handleGet req
       // a function that checks if req is directed to the server block
           // first retrieve the path to all servers in a list
           //Find the path that matches the req path
           //Once okay
           //Check if the req path points to a large file
               //If yes prepare a chunked response
           // otherwise prepare a standard response

class HTTPResponse {
    private:
        std::string statusCode;
        std::string statusMessage;
        std::string headers;
        std::string body;
        std::string response;

        void buildResponse(); // Builds the full HTTP response with headers and body

    public:
        HTTPResponse();
        void setStatus(const std::string& code, const std::string& message);
        void setResponse(const std::string& responseContent);
        
        // Generates a response from a file if it exists, otherwise sets a 404 response
        static HTTPResponse fromFile(const std::string& filePath);

        // Gets the full HTTP response data (headers + body)
        std::string getData();
};

#endif
