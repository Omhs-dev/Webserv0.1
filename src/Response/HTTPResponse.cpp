#include "HTTPResponse.hpp"

// Constructor with default 200 OK status
HTTPResponse::HTTPResponse() : _statusCode("200"), _statusMessage("OK") {
    _body = "<html><body><h1>Hello, World!</h1></body></html>";
}

// Generates response based on the method type and file path
HTTPResponse HTTPResponse::generateResponse(const std::string& method, const std::string& filePath) {
    HTTPResponse response;
    
    if (method == "GET") {
        response = HTTPResponse::fromFile(filePath);
    } else if (method == "POST") {
        response.setStatus("200", "OK");
        response.setBody("POST request received");
    } else if (method == "DELETE") {
        // Here, you can add file deletion logic
        response.setStatus("200", "OK");
        response.setBody("File deleted (simulated)");
    } else {
        response.setStatus("405", "Method Not Allowed");
        response.setBody("Only GET, POST, and DELETE methods are allowed");
    }
    return response;
}

// Generate a response from a file (used for GET requests)
HTTPResponse HTTPResponse::fromFile(const std::string &filePath) {
    HTTPResponse response;
    std::ifstream file(filePath);

    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();  // Read the file contents into a string
        response.setBody(buffer.str());

        // Set the content type based on the file extension
        if (filePath.find(".html") != std::string::npos) {
            response.setHeader("Content-Type", "text/html");
        } else if (filePath.find(".css") != std::string::npos) {
            response.setHeader("Content-Type", "text/css");
        } else if (filePath.find(".js") != std::string::npos) {
            response.setHeader("Content-Type", "application/javascript");
        } else {
            response.setHeader("Content-Type", "text/plain");
        }

        // Set content length
        response.setHeader("Content-Length", std::to_string(response._body.size()));
    } else {
        // File not found: return a 404 error
        response.setStatus("404", "Not Found");
        response.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", std::to_string(response._body.size()));
    }

    return response;
}

// Sets the response status code and message
void HTTPResponse::setStatus(const std::string& code, const std::string& message) {
    _statusCode = code;
    _statusMessage = message;
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value)
{
	_headers += key + ": " + value + "\r\n";
}

// Sets the response body content
void HTTPResponse::setBody(const std::string& body) {
    _body = body;
}


// Sets common headers for all responses
void HTTPResponse::setDefaultHeaders() {
    _headers = "Content-Type: text/html\r\n";
}

// Combines status line, headers, and body to create full HTTP response data
std::string HTTPResponse::getData() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
    oss << _headers;
    oss << "Content-Length: " << _body.size() << "\r\n";
    oss << "\r\n";
    oss << _body;
    return oss.str();
}
