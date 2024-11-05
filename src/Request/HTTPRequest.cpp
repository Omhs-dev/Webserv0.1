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
	_state = IS_REQUEST_LINE;
}

void HTTPRequest::parseRequest(const std::string &requestData)
{
	Logger::VerticalSeparator();
	Logger::Itroduction("parseRequest ↗️");
	Logger::VerticalSeparator();
	_rawRequest = requestData;
	Logger::Specifique(_rawRequest, "This is the raw request");
	std::istringstream stream(requestData);
	std::string line;

	while (std::getline(stream, line) && !line.empty())
	{
		// line = getLineSanitizer(stream);
		if (_state == IS_REQUEST_LINE)
		{
			parseRequestLine(line);
			_state = IS_HEADERS;
		}
		else if (_state == IS_HEADERS)
		{
			parseHeaderLine(line);
			_state = IS_HEADERS_END;
		}
	}
	if (!checkHostHeader())
	{
		Logger::NormalCout("Host header missing in HTTP/1.1 request");
		errorOccur(400); // Bad Request: Host header missing in HTTP/1.1 request
		return;
	}
	// // Parse the body for methods like POST
	_state = IS_BODY_START;
	if (_method == "POST")
	{
		std::string bodyData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		if (checkTransferEncoding() == 1)
			parseChunkedBody(bodyData);
		else if (checkContentLength() == 1)
			parseNormalBody(bodyData);
		_state = IS_BODY_END;
	}
	_state = COMPLETE;
}

void HTTPRequest::parseRequestLine(const std::string &line)
{
	// Logger::Itroduction("parseRequestLine 📜");
	// Logger::VerticalSeparator();
	std::stringstream ss(line);
	// _method = parseMethod(ss.str());
	// _uriPath = parsePath(ss.str());
	// _version = parseVersion(ss.str());attempting
	_state = IS_REQUEST_LINE;

	ss >> _method >> _uriPath >> _version;

	if (!checkMethod())
	{
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

void HTTPRequest::parseHeaderLine(const std::string &line)
{
	// Logger::Itroduction("parseHeaderLine 📰");
	// Logger::VerticalSeparator();
	// _state = IS_HEADERS;
	EnumState(_state);
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
	// else
	// {
	// 	Logger::NormalCout("Header line is invalid");
	// 	errorOccur(400);
	// }
	// _state = IS_HEADERS_END;
	EnumState(_state);
}

// void HTTPRequest::parseBody(std::istringstream stream)
// {
	
// }

void HTTPRequest::parseNormalBody(const std::string &bodyData)
{
	_state = IS_BODY_NORMAL;
	_body = bodyData.substr(0, _iscontentLength);
}

void HTTPRequest::parseChunkedBody(const std::string &bodyData)
{
	Logger::VerticalSeparator();
	Logger::Itroduction("parseChunkedBody");
	Logger::VerticalSeparator();
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

int HTTPRequest::checkMethod()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return 0;
	return 1;
}

int HTTPRequest::checkCgi()
{
	if (_location->getCgi().empty())
		return 0;
	return 1;
}

////////////////////////////////////////////////////
// ------------------- GETTERS -------------------//
////////////////////////////////////////////////////

Client *HTTPRequest::getClient() const { return _client; }

LocationConfig *HTTPRequest::getLocation() const { return _location; }

ServerConfig *HTTPRequest::getServer() const { return _server; }

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
	Logger::SpecifiqueForInt(_stateCode, "an error occured");
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