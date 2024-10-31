// #ifndef HTTPRESPONSE_HPP
// #define HTTPRESPONSE_HPP

// #include <string>
// #include <fstream>
// #include <sstream>
// #include <sys/stat.h>
// #include <vector>
// #include "../Server/Client.hpp"
// #include "../Request/HTTPRequest.hpp"

// // function that generates responses based on the req
//    // function to handle delete req
//    // function to handle put req
//    // function to handle get req
//    // for the function handleGet req
//        // a function that checks if req is directed to the server block
//            // first retrieve the path to all servers in a list
//            //Find the path that matches the req path
//            //Once okay
//            //Check if the req path points to a large file
//                //If yes prepare a chunked response
//            // otherwise prepare a standard response

// class Client;
// class HTTPRequest;

// class HTTPResponse {
//     private:
//         Client *_client;
//         HTTPRequest* _request;
		
//         std::string _statusCode;
//         std::string _statusMessage;
//         std::string _header;
//         std::string _body;
//         std::string _response;

//     public:
//         HTTPResponse();

//         //Setters
//         void setStatus(const std::string& code, const std::string& message);
//         void setDefaultResponse();
//         void setStandardResponse(const std::string &path);
//         void setHeader(const std::string& header);
//         void setBody(const std::string& body);

//         // HTTPRequest* getRequest() (return _request);
//         // Gets the full HTTP response data (headers + body)
//         std::string getData() const;
//         void generateResponse(const std::string &path);
//         void handleGetRequest(const std::string &path);

    
//         void setError(int code, bool errorPage);


// };

// #endif



// HTTPResponse.hpp
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

#include "../Request/HTTPRequest.hpp"
#include "../Server/Client.hpp"

#define MAX_ALLOWED_FILE_SIZE 1024 * 1024
#define MAX_RESPONSE_BODY_SIZE 1024 * 1024

class HTTPRequest;
class Client;

// Set a content type based on the file extension
// set an error page message

// Generate reponse
	// handle methodes
		// handle GET
			// check if the path is /
				// if yes serve by default index.html
			// if path is file or different of "/"
				// check if the path is a good path
					// if yes
						// check if the file is large
							// if yes prepare chunked response
						// check if the path is a file
							// if yes serve the file
						// check if the path is a directory
							// check if the directory has an index file
								// if yes serve the index file
							// check if the directory has an autoindex on or off
								// if yes serve the directory
						// if is a recirect
							// if yes serve the redirect
					// if no serve 404

class HTTPResponse
{
private:
	Client *_client;
	std::string _statusCode;
	std::string _statusMessage;
	std::vector<std::string> _headers;
	std::string _body;
	
	void handleGet(const std::string &path);
	void setDefaultResponse();
	void setStandardResponse(const std::string &path);
	void setStatus(const std::string &code, const std::string &message);
	void setBody(const std::string &body);

public:
	HTTPResponse();
	void generateResponse(const std::string &method, const std::string &path);
	std::string getData() const;
	std::vector<std::string> getHeaders() const { return _headers; }
	std::string getMimeType(const std::string &path);
	
	bool isFileLarge(const std::string &path);
	bool isFile(const std::string &path);
	bool isDirectory(const std::string &path);
	bool isPathValid(const std::string &path);

	void cleanPath(std::string &path);
	std::string listDirectory(const std::string &path, const std::string& root);
};

#endif
