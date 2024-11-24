#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(Client *client)
{
	_client = client;
	_location = nullptr;
	// _server = nullptr;
	_isChunked = false;
	_iscontentLength = 0;
	_timeout = 0;
	_stateCode = 0;
	_method = "";
	_uriPath = "";
	_version = "";
	_state = IS_REQUEST_LINE;
}

void HTTPRequest::parseRequest(const std::string &requestData)
{
	_rawRequest += requestData;
	// Logger::Specifique(_rawRequest, "Raw");
	std::istringstream stream(_rawRequest);
	std::string line;

	if (_state == COMPLETE)
	{
		Logger::NormalCout("Request already parsed");
		return;
	}

	// Parse Request Line and Headers
	bool headersParsed = false;
	while (std::getline(stream, line))
	{
		if (line == "\r")
		{
			headersParsed = true;
			break;
		}
		if (_state == IS_REQUEST_LINE)
		{
			parseRequestLine(line);
			_state = IS_HEADERS;
		}
		else if (_state == IS_HEADERS)
		{
			parseHeaders(line);
		}
	}

	if (!isMethodAllowed(_method, _uriPath))
	{
		Logger::ErrorCout("Method not allowed");
		errorOccur(405);
		return;
	}
	// check headers errors
	if (!headersParsed)
	{
		errorOccur(400);
		return;
	}
	if (!checkHostHeader())
	{
		Logger::ErrorCout("Host header missing in HTTP/1.1 request");
		errorOccur(400);
		return;
	}

	// Parse the body for methods for POST
	_state = IS_BODY_START;
	if (_method == "POST")
	{
		std::string bodyData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		if (checkFormData())
		{
			Logger::NormalCout("Post type multipart/form-data");

			size_t byte_count = bodyData.size() - 183;
			Logger::SpecifiqueForInt(byte_count, "body bytes");
			if (byte_count > getMaxbodySize())
			{
				errorOccur(413);
				_body = "";
				Logger::ErrorCout("Request Entity Too Large");
				return;
			}
		}
		if (checkTransferEncoding())
		{
			Logger::NormalCout("POST -> Transfer-Encoding : chunked");
			parseChunkedBody(bodyData);
		}
		_body = bodyData;
	}

	_state = COMPLETE;
}

void HTTPRequest::parseRequestLine(const std::string &line)
{
	_state = IS_REQUEST_LINE;

	std::stringstream ss(line);

	ss >> _method >> _uriPath >> _version;

	if (!checkHttpVersion())
	{
		Logger::ErrorCout("HTTP Version Not Supported");
		errorOccur(505);
		return;
	}
	if (_uriPath.find('?') != std::string::npos)
	{
		size_t pos = _uriPath.find('?');
		_query = _uriPath.substr(pos + 1);
		_uriPath = _uriPath.substr(0, pos);
	}
}

void HTTPRequest::parseHeaders(const std::string &line)
{
	if (line.empty())
	{
		Logger::ErrorCout("Header line is invalid");
		errorOccur(400);
		return;
	}

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
	}
}


void HTTPRequest::parseNormalBody(const std::string &bodyData)
{
	_state = IS_BODY_NORMAL;
	std::cout << "Is NORMAL BODY, no bodyshaming please\n";
	_body = bodyData;
}

void HTTPRequest::parseChunkedBody(const std::string &bodyData)
{
	std::stringstream ss(bodyData);
	std::string line;
	_state = IS_BODY_CHUNKED;
	EnumState(_state);

	while (std::getline(ss, line))
	{
		try
		{
			_chunkSize = std::stoi(line, nullptr, 16);
		}
		catch (const std::invalid_argument &)
		{
			errorOccur(400);
			break;
		}
		if (_isChunked && _headers.find("Content-Length") != _headers.end())
		{
			errorOccur(400);
			return;
		}
		if (_chunkSize == 0)
		{
			_state = COMPLETE;
			EnumState(_state);
			break;
		}

		std::string buffer(_chunkSize, '\0');
		ss.read(&buffer[0], _chunkSize);
		_body.append(buffer);

		if (ss.peek() == '\r')
			ss.ignore(2);
	}
	// _state = IS_BODY_END;
	EnumState(_state);
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
	return (_headers.find("Host") != _headers.end());
}

int HTTPRequest::checkFormUrlEncoded()
{
	auto it = _headers.find("Content-Type");
	if (it != _headers.end() && it->second == "application/x-www-form-urlencoded")
		return 1;
	return 0;
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

int HTTPRequest::checkFormData()
{
	auto it = _headers.find("Content-Type");
	if (it != _headers.end())
	{
		size_t space_pos = it->second.find(' ');
		std::string second = it->second.substr(0, space_pos);
		// Logger::Specifique(second, "Second Header");
		if (second == "multipart/form-data;")
			return 1;
	}
	return 0;
}

int HTTPRequest::checkContentLength()
{
	auto it = _headers.find("content-length");
	if (it != _headers.end())
	{
		try
		{
			_iscontentLength = std::stoull(it->second);
		}
		catch (const std::invalid_argument &)
		{
			errorOccur(400);
			return 0;
		}
		return 1;
	}
	return 0;
}

bool HTTPRequest::isMethodAllowed(const std::string& method, const std::string &path)
{
	const auto location = determineLocation(path);
    const std::vector<std::string>& allowedMethods = location.requestAllowed;
	if (location.requestAllowed.size() > 0)
	{
		for (auto &meth : allowedMethods)
		{
			Logger::Specifique(meth, "meth allowed");
			if (meth == _method)
				return true;
		}
		return false;
	}

    Logger::NormalCout("Path is not a server location. Continue...");
    return method == "GET" || method == "POST" || method == "DELETE";
}

int HTTPRequest::isCGI()
{
	if (_uriPath.find("/cgi-bin/") == std::string::npos)
		return (0);
	return 1;
}

////////////////////////////////////////////////////
// ------------------- GETTERS -------------------//
////////////////////////////////////////////////////

Client *HTTPRequest::getClient() const { return _client; }

LocationConfig *HTTPRequest::getLocation() const { return _location; }

std::string HTTPRequest::getMethod() const { return _method; }

std::string HTTPRequest::getPath() const { return _uriPath; }

std::string	HTTPRequest::getQuery() const { return _query; }

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

unsigned long long HTTPRequest::getMaxbodySize()
{
	std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	for (ServerConfig &server : configs)
	{
		if (server._clientMaxBodySize > 0 && server._clientMaxBodySize != 0)
		{
			Logger::SpecifiqueForInt(server._clientMaxBodySize, "client max body found");
			return server._clientMaxBodySize;
		}
	}
	return 0;
}


LocationConfig HTTPRequest::determineLocation(const std::string &path)
{
	ServerConfig server = determineServer();

	for (LocationConfig &location : server.getLocations())
	{
		if (path == location.locationPath)
		{
			// Logger::NormalCout("location found !");
			return location;
		}
	}

	return LocationConfig();
}

ServerConfig HTTPRequest::determineServer()
{
    std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	// std::vector<ServerConfig>::iterator iter = configs.begin();

    auto it = _headers.find("Host");
    if (it == _headers.end())
    {
        // Logger::ErrorCout("Host header not found!");
        return ServerConfig(); // Return the default server
    }

    std::string hostHeader = it->second; // e.g., localhost:8089
    std::string hostname, port;
    std::istringstream stream(hostHeader);
    if (std::getline(stream, hostname, ':') && std::getline(stream, port))
    {
        // Logger::Specifique(hostname, "Extracted hostname");
        // Logger::Specifique(port, "Extracted port");
    }

	for (std::vector<ServerConfig>::reverse_iterator iter = configs.rbegin(); iter != configs.rend(); ++iter)
    {
		if (std::stoi(port) == std::stoi(iter->_listen))
		{
			// Logger::NormalCout("server found !");
			return *iter;
		}
    }

    return ServerConfig();
}

void HTTPRequest::errorOccur(int code)
{
	_stateCode = code;
	_state = COMPLETE;
	EnumState(_state);
}

std::string HTTPRequest::getLineSanitizer(std::stringstream &ss)
{
	std::string line;
	std::getline(ss, line);
	if (line.find('\r') != std::string::npos)
		line.erase(line.find('\r'));
	return line;
}

void HTTPRequest::EnumState(HTTPRequest::ParseState state)
{
	std::string stateName;
	switch (state)
	{
	case HTTPRequest::START:
		stateName = "START";
		break;
	case HTTPRequest::IS_REQUEST_LINE:
		stateName = "REQUEST_LINE";
		break;
	case HTTPRequest::IS_METHODE:
		stateName = "METHOD";
		break;
	case HTTPRequest::IS_PATH:
		stateName = "PATH";
		break;
	case HTTPRequest::IS_VERSION:
		stateName = "VERSION";
		break;
	case HTTPRequest::IS_HEADERS:
		stateName = "HEADERS";
		break;
	case HTTPRequest::IS_HEADERS_END:
		stateName = "HEADERS_END";
		break;
	case HTTPRequest::IS_BODY_START:
		stateName = "BODY_START";
		break;
	case HTTPRequest::IS_BODY_NORMAL:
		stateName = "BODY_NORMAL";
		break;
	case HTTPRequest::IS_BODY_CHUNKED:
		stateName = "BODY_CHUNKED";
		break;
	case HTTPRequest::IS_BODY_END:
		stateName = "BODY_END";
		break;
	case HTTPRequest::COMPLETE:
		stateName = "COMPLETE";
		break;
	}
	Logger::Specifique(stateName, "State");
}