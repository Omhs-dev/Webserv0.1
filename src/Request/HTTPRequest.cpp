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
	Logger::Itroduction("parseRequest ↗️");
	_rawRequest += requestData;
	Logger::Specifique(_rawRequest, "RAW DATA");
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
		if (checkFormUrlEncoded()) // this is suppose to work the opposite way, on hold...
		{
			Logger::NormalCout("post type multipart/form-data");
			size_t byte_count = bodyData.size() - 183;
			Logger::SpecifiqueForInt(byte_count, "body bytes");
			if (byte_count > getMaxbodySize())
			{
				errorOccur(413);
				Logger::ErrorCout("Request Entity Too Large");
				return;
			}
		}
		// parseMultipartBody(bodyData);
		// Logger::Specifique(bodyData, "this is bodydata \n");

		_body = bodyData;
		Logger::Specifique(_body, "this is the body");
	}

	_state = COMPLETE;
}

void HTTPRequest::parseRequestLine(const std::string &line)
{
	Logger::Itroduction("parseRequestLine 📜");
	Logger::VerticalSeparator();
	std::stringstream ss(line);
	// _method = parseMethod(ss.str());
	// _uriPath = parsePath(ss.str());
	// _version = parseVersion(ss.str());attempting
	_state = IS_REQUEST_LINE;

	ss >> _method >> _uriPath >> _version;


	if (!checkLocMethodAllowed(_method, _uriPath))
	{
		Logger::NormalCout("Method not allowed");
		errorOccur(405);
		return;
	}
	if (!checkHttpVersion())
	{
		errorOccur(505); // HTTP Version Not Supported
		return;
	}
	if (_uriPath.find('?') != std::string::npos)
	{
		size_t pos = _uriPath.find('?');
		_query = _uriPath.substr(pos + 1);
		_uriPath = _uriPath.substr(0, pos);
	}
	Logger::NormalCout("Parsed Request Line : ");
	Logger::Specifique(_method, "Method");
	Logger::Specifique(_uriPath, "Path");
	Logger::Specifique(_version, "Version");
}

void HTTPRequest::parseHeaders(const std::string &line)
{
	// Logger::Itroduction("parseHeaders 📰");
	// Logger::VerticalSeparator();
	// _state = IS_HEADERS;
	// EnumState(_state);
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
	// Logger::VerticalSeparator();
	// Logger::Itroduction("parseChunkedBody");
	// Logger::VerticalSeparator();
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
			errorOccur(400); // Bad Request: Invalid chunk size
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
			errorOccur(400); // Bad Request: Invalid Content-Length
			return 0;
		}
		return 1;
	}
	return 0;
}

int HTTPRequest::checkLocMethodAllowed(const std::string &method, const std::string &path)
{
	std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	for (auto &server : configs)
	{
		for (LocationConfig &location : server.getLocations())
		{
			if (path == location.locationPath)
			{
				// Logger::Specifique(location.requestAllowed[0], "first method");
				// Logger::Specifique(location.requestAllowed[1], "second method");
				// Logger::NormalCout("Request path == location path");
				if (isMethodAllowed(method, location))
				{
					Logger::NormalCout("method allowed verified");
					return 1;
					// break;
				}
			}
		}
	}
	Logger::NormalCout("Request method is not found in location methods");
	return 0;
}

int HTTPRequest::isMethodAllowed(const std::string& method, const LocationConfig& location)
{
    const std::vector<std::string>& allowedMethods = location.requestAllowed;
    for (auto &meth : allowedMethods)
    {
        if (method == meth)
        {
            Logger::NormalCout("method found in config location");
            return 1;
        }
    }
    Logger::NormalCout("method nooot found in config location");
    return 0;
}

int HTTPRequest::isCGI()
{
	if (_uriPath.find("/cgi-bin/") == std::string::npos)
		return (std::cout << "Not a CGI requst" << std::endl, 0);
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
		if (server._clientMaxBodySize > 0)
		{
			Logger::SpecifiqueForInt(server._clientMaxBodySize, "client max body found");
			return server._clientMaxBodySize;
		}
	}
	return 0;
}

void HTTPRequest::errorOccur(int code)
{
	_stateCode = code;
	_state = COMPLETE;
	// Logger::SpecifiqueForInt(_stateCode, "an error occured");
	EnumState(_state);
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