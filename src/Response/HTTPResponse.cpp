#include "HTTPResponse.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

HTTPResponse::HTTPResponse() : _statusCode("200"), _statusMessage("OK") {}

void HTTPResponse::generateResponse(const std::string &method, const std::string &path)
{
	if (method == "GET")
	{
		handleGet(path);
	}
}

void HTTPResponse::handleGet(const std::string &path)
{
	if (path == "/")
	{
		setDefaultResponse();
	}
	else if (path != "/")
	{
		setStandardResponse("./www" + path);
		std::cout << "Path: " << path << std::endl;
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

void HTTPResponse::setStandardResponse(const std::string &path)
{
	std::ifstream file(path);

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		setStatus("200", "OK");
		setBody(buffer.str());
		
		file.close();
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

void HTTPResponse::setDefaultResponse()
{
	std::ifstream file("./www/index.html");

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		setBody(buffer.str());
		setStatus("200", "OK");
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}



void HTTPResponse::setStatus(const std::string &code, const std::string &message)
{
	_statusCode = code;
	_statusMessage = message;
}

void HTTPResponse::setBody(const std::string &body)
{
	_body = body;
}

std::string HTTPResponse::getData() const
{
	std::ostringstream oss;
	oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << _body.size() << "\r\n";
	oss << "\r\n";
	oss << _body;
	return oss.str();
}

// --- Checkers ---

bool HTTPResponse::isFile(const std::string &path)
{
	struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

bool HTTPResponse::isFileLarge(const std::string &path)
{
	std::ifstream file("./www" + path);
	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.close();
	return fileSize > MAX_ALLOWED_FILE_SIZE;
}

bool HTTPResponse::isDirectory(const std::string &path)
{
	struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool HTTPResponse::isPathValid(const std::string &path)
{
	for (ServerConfig serverConfig : _client->getServer()->getConfigs()._servers)
	{
		if (path.find(serverConfig._root) != std::string::npos)
		{
			return true;
		}
	}
	
	return false;
}

// --- Utils ---

std::string HTTPResponse::getMimeType(const std::string &path)
{
	std::string extension = path.substr(path.find_last_of('.') + 1);
	if (extension == "html" || extension == "htm")
	{
		return "text/html";
	}
	else if (extension == "css")
	{
		return "text/css";
	}
	else if (extension == "js")
	{
		return "text/javascript";
	}
	else
	{
		return "text/plain";
	}
}