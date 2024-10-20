#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest {
	public:
	    HTTPRequest();
	    void parseRequest(const std::string& requestData);
	    std::string getMethod() const;
	    std::string getPath() const;
	    std::string getVersion() const;
	
	private:
		int stateCode;
	    std::string method;
	    std::string path;
	    std::string version;
	    std::map<std::string, std::string> headers;
	
	    void parseRequestLine(const std::string& line);
	    void parseHeaderLine(const std::string& line);
};

#endif
