#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>

class HTTPResponse {
    std::string _status;
    std::string _body;

public:
    void setStatus(const std::string& code, const std::string& message) {
        _status = "HTTP/1.1 " + code + " " + message + "\r\n";
    }

    void setBody(const std::string& content) {
        _body = content;
    }

    std::string getResponse() const {
        return _status + "Content-Type: text/html\r\nContent-Length: " + 
               std::to_string(_body.size()) + "\r\n\r\n" + _body;
    }

    void setStandardResponse(const std::string &path);
};

// Cleans the directory path by handling "." and ".." and multiple slashes
void cleanPath(std::string& path) {
    if (path[0] != '/')
        path.insert(0, "/");
    if (path[path.size() - 1] != '/')
        path += "/";

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

// Lists the contents of a directory and returns an HTML-formatted string
std::string listDirectory(const std::string& path, const std::string& root) {
    std::string fullPath = root + path;
    DIR* dir = opendir(fullPath.c_str());

    if (!dir) {
        return ""; // Directory not found or unable to open
    }

    std::stringstream html;
    html << "<html><head><title>Directory Listing</title></head><body>";
    html << "<h1>Directory Listing for " << path << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        struct stat info;
        std::string itemPath = fullPath + "/" + name;
        if (stat(itemPath.c_str(), &info) == 0) {
            if (S_ISDIR(info.st_mode)) {
                html << "<li><b>[DIR]</b> <a href=\"" << path + "/" + name << "/\">" << name << "/</a></li>";
            } else {
                html << "<li><a href=\"" << path + "/" + name << "\">" << name << "</a></li>";
            }
        }
    }

    closedir(dir);
    html << "</ul></body></html>";
    return html.str();
}

// Sets the response based on the path provided
void HTTPResponse::setStandardResponse(const std::string& path) {
    std::string cleanPathStr = path;
    cleanPath(cleanPathStr);

    std::string directoryListing = listDirectory(cleanPathStr, "./www");
    if (!directoryListing.empty()) {
        setStatus("200", "OK");
        setBody(directoryListing);
    } else {
        setStatus("404", "Not Found");
        setBody("<html><body><h1>404 Not Found</h1></body></html>");
    }
}

// Main function to test the HTTPResponse
int main() {
    HTTPResponse response;
    std::string path = "/";  // Example path to test

    response.setStandardResponse(path);
    std::cout << response.getResponse() << std::endl;

    return 0;
}
