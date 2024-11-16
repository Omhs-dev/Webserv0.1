#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
# include <fcntl.h>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include "../Server/Client.hpp"
#include "../Parse/LocationConfig.hpp"
#include "../Parse/ServerConfig.hpp"
#include "../Parse/ConfigParser.hpp"
#include "../Server/Server.hpp"
#include "../Logger/Logger.hpp"


#define MAX_FILE_SIZE 512000
#define MAX_RESPONSE_BODY_SIZE 1000000

class HTTPRequest;
class Client;

/*
	↗️ ✔️ ✅ ❎ 🔗 ➡️ ▶️📄
*/


// we should be able to read a file in a listening directory✅

// restructure the listening directory folders and files ✅


// set an error page message ✅

// a function to check redrection url ✅

// if only / is requested on the navigator serve the index.html file ✅
	//remove the / from the path
	// example: localhost:8080/ -> localhost:8080

// create a serveFile function ✅
// create a custom error page to serve when a file is not found ✅

// Set a content type based on the file extension  ✅
// --------------------------------------------------------------------

// have an instance of Cgi
// a function to set the a chunked header

// Request for a Directory (Without Trailing Slash)
	// if a directory is requested whithout "/" at the end, the server should redirect to the same URL with a trailing slash
	// example: /directory -> /directory/

// Generate reponse
	// handle methodes
		// handle GET
			// check if the path is /
				// if yes serve by default index.html
			// if path is file or different of "/"
				// check if the path is a good path
					// if yes
						// check if the path is a file
							// if yes serve the file
						// check if the file is large
							// if yes prepare chunked response
						// check if the path is a directory
							// check if the directory has an index file
								// if yes serve the index file
							// check if the directory has an autoindex on or off
								// if yes serve the directory
							// check if directory has an alias
								// if yes set path to the alias - or create a function to handle that
						// if is a redirect
							// if yes serve the redirect
					// if no serve 404

class HTTPResponse
{
	public:
		enum ResponseState { INIT, IS_REDIRECT, IS_CHUNK, IS_FILE, IS_NORMAL, IS_ALIAS, COMPLETE };
	private:
		Client *_client;
		// Server *_server;
		HTTPRequest	*_request;
		ServerConfig *_server;
	
		ResponseState _state;
		std::string _statusCode;
		std::string _statusMessage;
		std::vector<std::string> _headers;
		std::string _body;
		std::string _errorPage;
		std::string _serverRoot;
		int _fileFd;
		
		void handleGet();
		void handleDelete();
		void handlePost();
		void serveFile(const std::string &path);
		void setDefaultResponse(std::string path, LocationConfig config);
		void setChunkResponse(const std::string &path);
		void setStandardResponse();
		void setServerRoot(const std::string &root);
		void setHeaders(const std::string &key, const std::string &value);
		void setStatus(const std::string &code, const std::string &message);
		void setBody(const std::string &body);

	public:
		HTTPResponse(Client *client);
		~HTTPResponse();
		
		void generateResponse();
		std::string getData() const;
		std::vector<std::string> getHeaders() const { return _headers; }
		// std::string getMimeType(const std::string &path);
		
		void cleanPath(std::string &path);
		std::string listDirectory(const std::string &path, const std::string& root);
		LocationConfig checkLocationPath(const std::string& path);
		ServerConfig checkServer(LocationConfig &location);
		
		std::string generateErrorPage(int code, ServerConfig server);
};

#endif
