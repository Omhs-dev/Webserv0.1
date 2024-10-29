#include "HTTPResponse.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

HTTPResponse::HTTPResponse() : statusCode("200"), statusMessage("OK") {}

void HTTPResponse::setStatus(const std::string& code, const std::string& message) {
    statusCode = code;
    statusMessage = message;
}

// void HTTPResponse::setBody(const std::string& bodyContent) {
//     body = bodyContent;
//     buildResponse();
// }

// HTTPResponse HTTPResponse::fromFile(const std::string& filePath) {
//     HTTPResponse response;
//     std::ifstream file(filePath);

//     if (file.is_open()) {
//         std::ostringstream bodyStream;
//         bodyStream << file.rdbuf();
//         response.body = bodyStream.str();
//         response.setStatus("200", "OK");
//     } else {
//         response.setStatus("404", "Not Found");
//         response.body = "<html><body><h1>404 Not Found</h1></body></html>";
//     }

//     response.buildResponse();
//     return response;
// }

// void HTTPResponse::buildResponse() {
//     std::string response = 
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/html\r\n"
//         "Content-Length: 20\r\n" // Length of the HTML content
//         "\r\n";
//     std::string bod = "<h1>Hello World</h1>";
//     headers = response;
//     setBody(bod);
// }

void HTTPResponse::setResponse(const std::string& responseContent) {
    response = responseContent;
}

std::string HTTPResponse::getData() {
    // Construct the response content
    body = "<h1>Hello World</h1>";
    std::string responseContent = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;
    
    // Use the setter to ensure _response is updated
    setResponse(responseContent);

    // Return the stored response
    return response;
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