#include "HTTPRequest.hpp"
#include "../Server/Client.hpp"
#include <sstream>
#include <iostream>

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
	_maxBody = 0;
	_state = IS_REQUEST_LINE;
}

void HTTPRequest::parseRequest(const std::string &requestData)
{
	// Logger::VerticalSeparator();
	Logger::Itroduction("parseRequest ↗️");
	// Logger::VerticalSeparator();
	_rawRequest = requestData;
	std::cout << "RAW DATA\n" << _rawRequest << std::endl;
	// Logger::Specifique(_rawRequest, "This is the raw request");
	std::istringstream stream(_rawRequest);
	std::string line;

	if (_state == COMPLETE)
	{
		// Logger::NormalCout("Request already parsed");
		return;
	}

	while (std::getline(stream, line) && !line.empty())
	{
		if (_state == IS_REQUEST_LINE)
		{
			parseRequestLine(line);
			_state = IS_HEADERS;
		}
		else if (_state == IS_HEADERS)
		{
			parseHeaderLine(line);
		}
		if (line.find("Priority:") != std::string::npos)
			break ;
	}
	for (auto &header : _headers)
	{
		std::cout << "Key: " << header.first << ", Value: " << header.second << std::endl;
	}
	if (!checkHostHeader())
	{
		// Logger::NormalCout("Host header missing in HTTP/1.1 request");
		errorOccur(400); // Bad Request: Host header missing in HTTP/1.1 request
		return;
	}
	if (checkMaxBodySize())
	{
		errorOccur(413);
		Logger::ErrorCout("Request Entity Too Large");
		return;
	}
	// // Parse the body for methods like POST
	_state = IS_BODY_START;
	if (_method == "POST")
	{
		// Logger::NormalCout("POST method detected");
		std::string bodyData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		_body = bodyData;
		// if (_headers.find("Content-Type") != _headers.end() && _headers["Content-Type"].find("multipart/form-data") != std::string::npos)
		// {
		// 	std::cout << "Found correct content type\n";
		// 	parseMultipartBody(bodyData);
		// }
		// if (checkTransferEncoding() == 1)
		// 	parseChunkedBody(bodyData);
		// else
		// 	parseNormalBody(bodyData);
		// _state = IS_BODY_END;
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
	// Logger::NormalCout("Parsed Request Line : ");
	// Logger::Specifique(_method, "Method");
	// Logger::Specifique(_uriPath, "Path");
	// Logger::Specifique(_version, "Version");
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
	}
	// else
	// {
		//Logger::NormalCout("Header line is invalid");
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
	std::cout << "Is NORMAL BODY, no bodyshaming please\n";
	_body = bodyData.substr(0, _iscontentLength);
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
// ------------------- UPLOADS -------------------//
////////////////////////////////////////////////////

std::string HTTPRequest::getBoundary()
{
	auto it = _headers.find("Content-Type");
	if (it != _headers.end())
	{
		size_t pos = it->second.find("boundary=");
		if (pos != std::string::npos)
			return it->second.substr(pos + 9);
	}
	return "";
}

// void HTTPRequest::parseMultipartBody(const std::string &bodyData)
// {
// 	std::string boundary = getBoundary();
// 	if (boundary.empty())
// 	{
// 		errorOccur(400); // Bad Request: Invalid boundary
// 		return;
// 	}
// 	std::string delimiter = "--" + boundary;
// 	std::string endDelimiter = "--" + boundary + "--";
// 	size_t pos = bodyData.find(delimiter);
// 	if (pos == std::string::npos)
// 	{
// 		errorOccur(400); // Bad Request: Invalid boundary
// 		return;
// 	}
// 	pos += delimiter.length();
// 	while (pos != std::string::npos)
// 	{
// 		size_t endPos = bodyData.find(delimiter, pos);
// 		if (endPos == std::string::npos)
// 			break;
// 		std::string part = bodyData.substr(pos, endPos - pos);
// 		size_t headerPos = part.find("\r\n\r\n");
// 		if (headerPos == std::string::npos)
// 			break;
// 		std::string header = part.substr(0, headerPos);
// 		std::string content = part.substr(headerPos + 4);
// 		std::istringstream stream(header);
// 		std::string line;
// 		std::string key;
// 		std::string value;
// 		while (std::getline(stream, line) && !line.empty())
// 		{
// 			size_t colonPos = line.find(':');
// 			if (colonPos != std::string::npos)
// 			{
// 				key = line.substr(0, colonPos);
// 				value = line.substr(colonPos + 1);
// 				std::string::iterator it = value.begin();
// 				while (it != value.end() && std::isspace(*it))
// 					it++;
// 				value.erase(value.begin(), it);
// 			}
// 		}
// 		// Save the file
// 		std::string filename;
// 		std::string::size_type pos = key.find("filename=");
// 		if (pos != std::string::npos)
// 		{
// 			filename = key.substr(pos + 10);
// 			filename.erase(filename.end() - 1);
// 			std::ofstream file(filename, std::ios::binary);
// 			file << content;
// 			file.close();
// 		}
// 		pos = endPos + delimiter.length();
// 	}
// }

void HTTPRequest::saveFile(const std::string &filename, const std::string &content)
{
	std::ofstream file("./uploads/" + filename, std::ios::binary);
	if (file.is_open())
	{
		// Logger::NormalCout("File opened successfully");
		file << content;
		file.close();
	}
	// else
	// 	Logger::NormalCout("File failed to open");
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

int HTTPRequest::checkMaxBodySize()
{
	setMaxbodySize(_maxBody);
	Logger::SpecifiqueForInt(_iscontentLength, "body length");
	Logger::SpecifiqueForInt(_maxBody, "max body set successfully..");
	if (_iscontentLength > _maxBody)
	{
		Logger::SpecifiqueForInt(_iscontentLength, "content legth size checkMaxBodySize");
		Logger::SpecifiqueForInt(_maxBody, "maxBody size checkMaxBodySize");
		Logger::NormalCout("Content too large !");
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

void HTTPRequest::setMaxbodySize(unsigned long long size)
{
	std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	for (ServerConfig &server : configs)
	{
		if (server.getClientMaxBodySize() > 0)
		{
			Logger::SpecifiqueForInt(server.getClientMaxBodySize(), "client max body found");
			size = server.getClientMaxBodySize();
			Logger::SpecifiqueForInt(size, "size set at ");
			break;
		}
	}
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
	// Logger::Specifique(stateName, "State");
}