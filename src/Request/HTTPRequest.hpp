#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest {
	private:
		// Request Line
	    std::string method;
	    std::string path;
	    std::string _uri;
	    std::string version;
	    std::string _query;
	    std::string _rawRequest;
	    std::string _body;
	    // Headers
	    std::map<std::string, std::string> headers;
	    std::string _headerKey;
	    std::string _headerValue;
	    bool _isChunked;
	    std::string _contentLength;
	    unsigned long long _iscontentLength;
	    int _chunkSize;
	    time_t _timeout;
	    int _stateCode;
	    
		// Parse request
	    void parseRequestLine(const std::string& line);
	    void parseHeaderLine(const std::string& line);
	    std::string parseMethod(const std::string& line);
	    std::string parsePath(const std::string& line);
	    std::string parseVersion(const std::string& line);
	
	public:
	    HTTPRequest();
	    void parseRequest(const std::string& requestData);
	    std::string getMethod() const;
	    std::string getPath() const;
	    std::string getVersion() const;
};

#endif
