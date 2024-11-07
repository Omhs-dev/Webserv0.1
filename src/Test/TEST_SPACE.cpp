#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> 
#include <sys/stat.h> // For unlink (to delete files)

// Assuming these are part of your project
class HTTPRequest {
public:
    std::string getPath() const { return path; }
    void setPath(const std::string &p) { path = p; }

private:
    std::string path;
};

class HTTPResponse {
public:
    HTTPResponse(HTTPRequest *req) : _request(req) {}

    void handleDelete();

    // Set body and print response for testing purposes
    void setBody(const std::string &body) { _body = body; }
    void setState(int state) { _state = state; }

    void printResponse() const { std::cout << _body << std::endl; }

private:
    HTTPRequest *_request;
    std::string _body;
    int _state;

    bool isFile(const std::string &path) {
        struct stat fileStat;
        return (stat(path.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode));
    }

    bool isDirectory(const std::string &path) {
        struct stat fileStat;
        return (stat(path.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode));
    }

    std::string getErrorMessage(int code) {
        if (code == 404)
            return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        if (code == 403)
            return "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
        return "";
    }

    std::string intToString(size_t num) {
        return std::to_string(num);
    }
};

// Define handleDelete based on the previous implementation
void HTTPResponse::handleDelete() {
    std::string reqPath = _request->getPath();
    if (!isFile(reqPath)) {
        _body = getErrorMessage(404);
        return;
    }
    if (isDirectory(reqPath) || remove(reqPath.c_str()) != 0) {
        _body = getErrorMessage(403);
        return;
    }
    std::string jsonBody = "{\n";
    jsonBody += "  \"message\": \"File deleted successfully.\",\n";
    jsonBody += "  \"filename\": \"" + reqPath + "\"\n";
    jsonBody += "}\n";

    _body = "HTTP/1.1 200 OK\r\n";
    _body += "Content-Type: application/json\r\n";
    _body += "Content-Length: " + intToString(jsonBody.size()) + "\r\n";
    _body += "\r\n";
    _body += jsonBody;
}

// Test main function
int main() {
    // Create a temporary file for testing
    const std::string tempFilePath = "testfile.txt";
    std::ofstream outFile(tempFilePath);
    outFile << "Temporary file content" << std::endl;
    outFile.close();

    // Verify file creation
    if (!outFile) {
        std::cerr << "Failed to create test file." << std::endl;
        return 1;
    }

    // Prepare HTTP request for the DELETE operation
    HTTPRequest request;
    request.setPath(tempFilePath);

    // Process DELETE request
    HTTPResponse response(&request);
    response.handleDelete();

    // Print the response (should indicate success if the file was deleted)
    response.printResponse();

    // Verify deletion
    if (access(tempFilePath.c_str(), F_OK) == -1) {
        std::cout << "File deletion verified successfully." << std::endl;
    } else {
        std::cerr << "File deletion failed." << std::endl;
    }

    return 0;
}
