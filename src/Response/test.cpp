#include <iostream>
#include <string>
#include <vector>

// Mock Server Class
class Server {
public:
    std::string getRoot() const {
        return "/var/www/html"; // Example server root
    }
};

// Mock Location Class
class Location {
private:
    std::string root;
    std::string alias;
    std::vector<std::string> indexes;
    std::string path;

public:
    Location(std::string root, std::string alias, std::vector<std::string> indexes, std::string path)
        : root(root), alias(alias), indexes(indexes), path(path) {}

    std::string getRoot() const { return root; }
    std::string getAlias() const { return alias; }
    std::vector<std::string> getIndexes() const { return indexes; }
    std::string getPath() const { return path; }
};

// Mock HTTPRequest Class
class HTTPRequest {
private:
    std::string path;
    Location* location;
    Server* server;

public:
    HTTPRequest(std::string path, Location* location, Server* server)
        : path(path), location(location), server(server) {}

    std::string getPath() const { return path; }
    Location* getLocation() const { return location; }
    Server* getServer() const { return server; }
};

// HTTPResponse Class with the getAllPathsLoc Method
class HTTPResponse {
public:
    HTTPRequest* request;

    HTTPResponse(HTTPRequest* req) : request(req) {}

    std::vector<std::string> getAllPathsLoc() {
        std::vector<std::string> allPathsLocation; 
        std::string path = this->request->getPath();
        std::string root = this->request->getLocation()->getRoot();
        std::string alias = this->request->getLocation()->getAlias();
        std::vector<std::string> indexes = this->request->getLocation()->getIndexes();
        bool isAlias = false;

        if (this->request->getLocation() == nullptr)
            return std::vector<std::string>(); // Return empty if no location

        if (root.empty()) 
            root = this->request->getServer()->getRoot();
        
        if (!alias.empty()) {
            root = alias;
            isAlias = true;
        }

        // If the request is for a specific file (i.e., doesn't end with "/")
        if (path.back() != '/') {
            if (isAlias && this->request->getLocation()->getPath().size() <= path.size()) {
                path = path.substr(this->request->getLocation()->getPath().size());
            }
            allPathsLocation.push_back(root + path);
        }

        // Iterate over the indexes and create possible paths
        for (size_t i = 0; i < indexes.size(); i++) {
            std::string index = indexes[i];
            std::string bkpPath = path; // Backup original path

            // If the path is root "/"
            if (path == "/") {
                path = root + "/" + index;
            } else if (isAlias) {
                path = root + "/" + index; // Fix path by adding '/'
            } else {
                path = root + path + index; // Fix path by adding '/'
            }

            allPathsLocation.push_back(path); // Add the generated path
            path = bkpPath; // Restore original path
        }

        return allPathsLocation;
    }
};

struct LocationInfo
{
	std::string	_path;
    std::string _root;
    std::string _alias;
    std::vector<std::string> _indexes;
    std::vector<e_Methods> _allowedMethods;
    bool _autoindex;
    // std::map<std::string, std::string> _cgiExtension;
    std::string _uploadPath;
};

void manageLocation(void)
{
    std::vector<LocationInfo> collectLocations;
    collectLocations = this->request->getLocations();
    bool isAlias = false;
    for (size_t i = 0; i < collectLocation.size(); i++)
    {
        std::string path = collectLocation[i].getPath();
        std::string root = collectLocation[i].getRoot();
        std::string alias = collectLocation[i].getAlias();
        

    }
    

}


