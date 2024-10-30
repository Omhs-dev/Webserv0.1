#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

#include "../Server/Client.hpp"
#include "../Parse/LocationConfig.hpp"
#include "../Parse/ServerConfig.hpp"

#define REQUEST_DEFAULT_STATE_CODE 200


class Client;

class HTTPRequest
{
	friend class HTTPRequestTest;
	private:
		Client          *_client;
		LocationConfig  *_location;
		ServerConfig    *_server;
		// HTTPConfigs     _httpConfigs;
		
		// Request Line
		std::string     _method;
		std::string     _uriPath;
		std::string     _version;
		
		// Headers
		std::string     _query;
		std::string     _rawRequest;
		std::string     _body;
		std::map<std::string, std::string>  _headers;
		std::string     _headerKey;
		std::string     _headerValue;
		bool            _isChunked;
		std::string     _contentLength;
		unsigned long long _iscontentLength;
		int             _chunkSize;
		time_t          _timeout;
		int             _stateCode;
		
		 // Parsing states
        enum ParseState { REQUEST_LINE, HEADERS, BODY, COMPLETE };
        ParseState _state;
	
		// --- Parse Request Line ---
		void        parseRequestLine(const std::string &line);
		std::string parseMethod(const std::string &line);
		std::string parsePath(const std::string &line);
		std::string parseVersion(const std::string &line);
		
		// --- Parse Headers ---
		void        parseHeaderLine(const std::string &line);
		std::string parseHeaderKey(const std::string &line);
		std::string parseHeaderValue(const std::string &line);
		
		// --- Parse Body ---
		void        parseBody(const std::string &line);
		void        parseChunkedBody(const std::string &line);
		
		
		// --- UTILS ---
		int processPath();
		void defineBodyDestination();
		std::vector<std::string> getAllPathsLocation();
		

	public:
		HTTPRequest(Client *client);
		void parseRequest(const std::string &requestData);
		
		// --- CHECKERS ---
		bool checkHttpVersion();
		bool checkHostHeader();
		int 	   checkTransferEncoding();
		int 	  checkContentLength();
		int checkMethod();
		int checkCgi();
		// --- GETTERS ---
		Client          *getClient() const;
		LocationConfig  *getLocation() const;
		ServerConfig    *getServer() const;
		
		// --- Request Line ---
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		// --- Headers ---
		
		std::map<std::string, std::string> getHeaders() const;
		std::string getHeaderKey() const;
		std::string getHeaderValue() const;
		std::string getBody() const;
		std::string getQuery() const;
		std::string getRawRequest() const;
		bool getIsChunked() const;
		unsigned long long getContentLength() const;
		int getChunkSize() const;
		time_t getTimeout() const;
		int getStateCode() const;
		

		//Setters
		void setStateCode(int stateCode) {_stateCode = stateCode;}
		// --- UTILS ---
		std::string getLineSanitizer(std::stringstream &ss);
		void errorOccur(int code);
};

#endif
