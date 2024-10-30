#include "HTTPRequest.hpp"
#include "../Server/Client.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest(Client *client)
{
	_client = client;
	_location = nullptr;
	_server = nullptr;
	_isChunked = false;
	_iscontentLength = 0;
	_timeout = 0;
	_stateCode = 0;
	_method = "";
	_uriPath = "";
	_version = "";
	_state = REQUEST_LINE;
}

void HTTPRequest::parseRequest(const std::string &requestData)
{
    _rawRequest = requestData;
    std::istringstream stream(requestData);
    std::string line;

    while (std::getline(stream, line) && !line.empty())
    {
        // line = getLineSanitizer(stream);
        if (_state == REQUEST_LINE)
        {
            parseRequestLine(line);
            _state = HEADERS;
        }
        else if (_state == HEADERS)
        {
            parseHeaderLine(line);
        }
    }

    if (!checkHostHeader())
    {
        errorOccur(400);  // Bad Request: Host header missing in HTTP/1.1 request
    }

    // Parse the body for methods like POST
    if (_method == "POST" && _state == HEADERS)
    {
        std::string bodyData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        if (checkTransferEncoding() == 1)
            parseChunkedBody(bodyData);
        else if (checkContentLength() == 1)
            parseBody(bodyData);
    }

    _state = COMPLETE;
}

void HTTPRequest::parseRequestLine(const std::string &line)
{
	std::stringstream ss(line);
	_method = parseMethod(ss.str());
	_uriPath = parsePath(ss.str());
	_version = parseVersion(ss.str());
	
	if (checkHttpVersion() == false)
	{   
		errorOccur(505);  // HTTP Version Not Supported
        return;
	}

	if (_uriPath.find('?') != std::string::npos) {
        size_t pos = _uriPath.find('?');
        _query = _uriPath.substr(pos + 1);
        _uriPath = _uriPath.substr(0, pos);
    }
	std::cout << "Parsed Request Line: " << _method << " " << _uriPath << " " << _version << std::endl;
}

void HTTPRequest::parseHeaderLine(const std::string &line)
{
	size_t colonPos = line.find(':');
	if (colonPos != std::string::npos)
	{
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		std::string::iterator it = value.begin();
		while (it != value.end() && std::isspace(*it))
			it++;
		value.erase(value.begin(), it);
		_headers[key] = value;
		std::cout << "Parsed Header: " << key << ": " << value << std::endl;
	}
}
// void HTTPRequest::parseHeaderLine(const std::string &line)
// {
//     std::string key = parseHeaderKey(line);
//     std::string value = parseHeaderValue(line);
//     _headers[key] = value;
// }

std::string HTTPRequest::parseMethod(const std::string &line)
{
    std::stringstream ss(line);
    std::string method;
    ss >> method;
    // if (checkMethod() == 0)
	// {
	// 	errorOccur(405);  // Method Not Allowed
	// 	return "";
	// }
    return method;
}

std::string HTTPRequest::parsePath(const std::string &line)
{
    std::stringstream ss(line);
    std::string method, path;
    ss >> method >> path;
    return path;
}

std::string HTTPRequest::parseVersion(const std::string &line)
{
    std::stringstream ss(line);
    std::string method, path, version;
    ss >> method >> path >> version;
    return version;
}

void HTTPRequest::parseBody(const std::string &bodyData)
{
    _body = bodyData;
}

void HTTPRequest::parseChunkedBody(const std::string &bodyData)
{
    std::stringstream ss(bodyData);
    std::string line;

    while (std::getline(ss, line))
    {
        try {
            _chunkSize = std::stoi(line, nullptr, 16);
        } catch (const std::invalid_argument&) {
            errorOccur(400);  // Bad Request: Invalid chunk size
            break;
        }

        if (_chunkSize == 0)
        {
            _state = COMPLETE;
            break;
        }

        std::string buffer(_chunkSize, '\0');
        ss.read(&buffer[0], _chunkSize);
        _body.append(buffer);

        if (ss.peek() == '\r') ss.ignore(2); // Ignore \r\n after the chunk
    }
}

////////////////////////////////////////////////////
// ------------------- CHECKERS ------------------//
////////////////////////////////////////////////////

bool HTTPRequest::checkHttpVersion()
{
    return (_version == "HTTP/1.1" || _version == "HTTP/1.0");
}

bool HTTPRequest::checkHostHeader()
{
    return _headers.find("host") != _headers.end();
}

int HTTPRequest::checkTransferEncoding()
{
    auto it = _headers.find("transfer-encoding");
    if (it != _headers.end() && it->second == "chunked")
    {
        _isChunked = true;
        return 1;
    }
    return 0;
}

int HTTPRequest::checkContentLength()
{
    auto it = _headers.find("content-length");
    if (it != _headers.end())
    {
        try {
            _iscontentLength = std::stoull(it->second);
        } catch (const std::invalid_argument&) {
            errorOccur(400);  // Bad Request: Invalid Content-Length
            return 0;
        }
        return 1;
    }
    return 0;
}

int HTTPRequest::checkMethod()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		return 0;
	}
	return 1;
}

int HTTPRequest::checkCgi()
{
	if (_location->getCgi().empty()) {
		return 0;
	}
	return 1;
}


////////////////////////////////////////////////////
// ------------------- GETTERS -------------------//
////////////////////////////////////////////////////

Client* HTTPRequest::getClient() const { return _client; }

LocationConfig* HTTPRequest::getLocation() const { return _location; }

ServerConfig* HTTPRequest::getServer() const { return _server; }

std::string HTTPRequest::getMethod() const { return _method; }

std::string HTTPRequest::getPath() const { return _uriPath; }

std::string HTTPRequest::getVersion() const { return _version; }

std::map<std::string, std::string> HTTPRequest::getHeaders() const { return _headers; }

std::string HTTPRequest::getBody() const { return _body; }

bool HTTPRequest::getIsChunked() const { return _isChunked; }

unsigned long long HTTPRequest::getContentLength() const { return _iscontentLength; }

time_t HTTPRequest::getTimeout() const { return _timeout; }

int HTTPRequest::getStateCode() const { return _stateCode; }

////////////////////////////////////////////////////
// ------------------- UTILS -------------------- //
////////////////////////////////////////////////////

void HTTPRequest::errorOccur(int code)
{
    _stateCode = code;
    _state = COMPLETE;
}

// make sure to remove the '\r' character from the line
std::string HTTPRequest::getLineSanitizer(std::stringstream &ss)
{
    std::string line;
    std::getline(ss, line);
    if (line.find('\r') != std::string::npos)
        line.erase(line.find('\r'));
    return line;
}

LocationConfig* HTTPRequest::findLocationConfig(const std::string& path) {
    // Ensure server and locations are correctly initialized
    auto& locations = _client->getServer()->getConfigs()._servers[0]._locations;
    
    for (LocationConfig& location : locations) {
        std::string locPath = location.getLocationPath();

        // Check if locPath is empty or contains invalid data
        if (locPath.empty() || locPath.find_first_not_of(" \t\n") == std::string::npos) {
            std::cerr << "Warning: Empty or invalid location path in config." << std::endl;
            continue;
        }

        std::cout << "Checking location path: " << locPath << std::endl;
        std::cout << "Client server root: " << _client->getServer()->getConfigs()._servers[0]._root << std::endl;

        // Check if the location path is a prefix of the request path
        if (path.find(locPath) == 0) {
            std::cout << "Match found for path: " << locPath << std::endl;
            return &location;  // Found a matching location
        }
    }

    std::cerr << "Error: Location not configured for path: " << path << std::endl;
    return nullptr;  // No matching location found
}


bool HTTPRequest::validateLocation(const std::string& path) {
    LocationConfig* location = findLocationConfig(path);
    if (location == nullptr) {
        std::cerr << "Error: Location not configured for path: " << path << std::endl;
        return false;  // Indicate invalid location
    }
    _location = location;  // Store the valid location
    return true;
}
