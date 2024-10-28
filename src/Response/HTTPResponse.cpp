#include "HTTPResponse.hpp"

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
