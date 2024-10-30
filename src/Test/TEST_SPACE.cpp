#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

void cleanPath(std::string& path) {
    // Ensure path starts with '/' and ends with '/'
    if (path[0] != '/')
        path.insert(0, "/");
    if (path[path.size() - 1] != '/')
        path += "/";

    // Normalize multiple slashes to a single slash
    path.erase(std::unique(path.begin(), path.end(), [](char a, char b) {
        return a == '/' && b == '/';
    }), path.end());

    // Remove all "/./" occurrences
    size_t pos;
    while ((pos = path.find("/./")) != std::string::npos) {
        path.erase(pos, 2);
    }

    // Remove all "/prev/../" occurrences
    while ((pos = path.find("/../")) != std::string::npos) {
        if (pos == 0) {
            path.erase(0, 3);
            continue;
        }
        size_t prev = path.rfind('/', pos - 1);
        if (prev != std::string::npos) {
            path.erase(prev, pos - prev + 3);
        } else {
            path.erase(0, pos + 3);
        }
    }
}



// Assume cleanPath, listDirectory, and HTTPResponse are defined elsewhere
// void cleanPath(std::string& path);

class HTTPResponse {
public:
    void setResponse(const std::string& responseContent) { _response = responseContent; }
    std::string getData() const {
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "<h1>Hello World</h1>";
        const_cast<HTTPResponse*>(this)->setResponse(response);
        return _response;
    }
private:
    std::string _response;
};

// Mock function to test listDirectory
std::string listDirectory(std::string path, std::string root) {
    cleanPath(path);

    // Ensure path starts with a '.' if it doesn't already
    if (path[0] != '.')
        path.insert(0, ".");

    // Output debugging information
    std::cout << "Root: " << root << std::endl;
    std::cout << "Path: " << path << std::endl;

    return ""; // Placeholder return, replace with actual return logic
}

int main() {
    // Test 1: cleanPath function
    std::vector<std::string> paths = {
        "/Users/toukoum/42/webserv/././www/../www/../",
        "/usr/local/../bin/./",
        "relative/./path/../to/test/",
        "/already/clean/path/"
    };

    std::cout << "Testing cleanPath function:\n";
    for (std::string& path : paths) {
        std::string original = path;
        cleanPath(path);
        std::cout << "Original: " << original << "\nCleaned:  " << path << "\n\n";
    }

    // Test 2: listDirectory function
    std::string root = "/root";
    std::string pathToDir = "/some/./directory/../path";
    std::cout << "Testing listDirectory function:\n";
    std::cout << listDirectory(pathToDir, root) << "\n\n";

    // Test 3: HTTPResponse's getData function
    HTTPResponse response;
    std::cout << "Testing HTTPResponse getData function:\n";
    std::cout << response.getData() << "\n";

    return 0;
}
