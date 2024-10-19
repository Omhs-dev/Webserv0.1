#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest {
	private:
		// Request Line
	    std::string _method;
	    std::string _uriPath;
	    std::string _version;
	    // Headers
	    std::string _query;
	    std::string _rawRequest;
	    std::string _body;
	    std::map<std::string, std::string> _headers;
	    std::string _headerKey;
	    std::string _headerValue;
	    bool _isChunked;
	    std::string _contentLength;
	    unsigned long long _iscontentLength;
	    int _chunkSize;
	    time_t _timeout;
	    int _stateCode;
	    
		// Parse request
		// --- Request Line ---
	    void parseRequestLine(const std::string& line);
	    std::string parseMethod(const std::string& line);
	    std::string parsePath(const std::string& line);
	    std::string parseVersion(const std::string& line);
	    // --- Headers ---
	    void parseHeaderLine(const std::string& line);
	    std::string parseHeaderKey(const std::string& line);
	    std::string parseHeaderValue(const std::string& line);
	
	public:
	    HTTPRequest();
	    void parseRequest(const std::string& requestData);
		// --- Request Line ---
	    std::string getMethod() const;
	    std::string getPath() const;
	    std::string getVersion() const;
	    // --- Headers ---
	    std::map<std::string, std::string> getHeaders() const;
	    std::string getHeaderKey() const;
	    std::string getHeaderValue() const;
};

#endif