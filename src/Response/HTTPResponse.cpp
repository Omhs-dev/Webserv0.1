#include "HTTPResponse.hpp"
#include <fstream>
#include <sstream>
#include <iostream>


HTTPResponse::HTTPResponse() {}

// void HTTPResponse::setStatus(const std::string& code, const std::string& message) {
//     statusCode = code;
//     statusMessage = message;
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
//     std::ostringstream responseStream;
//     responseStream << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
//     responseStream << "Content-Length: " << body.size() << "\r\n";
//     responseStream << "Content-Type: text/html\r\n";
//     responseStream << "\r\n";
//     responseStream << body;

//     headers = responseStream.str();
// }

// std::string HTTPResponse::getData() const {
//     return headers + body;
// }


bool fileExist(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool HTTPResponse::isLargeFile(const std::string &path)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) != 0) // 'stat' still needs C-style string
    {
        std::cerr << "Error: Failed to stat file: " << path << std::endl;
        return false;
    }
    return fileStat.st_size > THRESHOLD_LARGE_FILE;
}


std::string HTTPResponse::findGoodPath(std::vector<std::string> allpaths)
{
    for (size_t i = 0; i < allpaths.size(); i++)
    {
        std::cout << "Trying to open file: " << allpaths[i] << std::endl;
        if (fileExist(allpaths[i]))
            return allpaths[i];  // Return the first valid file path
    }
    return "";  // Return empty string if no valid file path found
}


std::vector<std::string> HTTPResponse::getAllPathsLoc()
{
    std::vector<std::string> allPathsLocation
    std::string path = this->HTTPRequest->getPath();
    std::string root = this->HTTPRequest->getLocation()->getRoot();
    std::string alias = this->HTTPRequest->getLocation()->getAlias();
    std::vector<std::string> indexes = this->HTTPRequest->getLocation()->getIndexes();
    bool isAlias = false;

    if (this->HTTPRequest->getLocation() == NULL)
        return std::vector<std::string>(); // Return empty if no location
    if (root.empty()) // Set root to server's root if it's empty
        root = this->HTTPRequest->getServer()->getRoot();
    if (!alias.empty()) {
        root = alias;
        isAlias = true;
    }
    // If the request is for a specific file (i.e., doesn't end with "/")
    if (path.back() != '/') {
        if (isAlias)
            path = path.substr(this->HTTPRequest->getLocation()->getPath().size());
        allPathsLocation.push_back(root + path); // Fix push_back target
    }
    // Iterate over the indexes and create possible paths
    for (size_t i = 0; i < indexes.size(); i++) {
        std::string index = indexes[i];
        std::string bkpPath = path; // Backup original path
        // If the path is root "/"
        if (path == "/") {
            path = root + "/" + index;
        } else if (isAlias) {
            path = root + "/" + index;
        } else {
            path = root + path + index;
        }
        allPathsLocation.push_back(path); // Add the generated path
        path = bkpPath; // Restore original path
    }
    return allPathsLocation;
}


//Checking if the request is looking for a location bloc,
//then manage it
void HTTPResponse::manageLocation()
{
    std::string root;
    this->HTTPRequest->getLocation()->getRoot().empty() ? root = this->HTTPRequest->getServer()->getRoot() : root = this->HTTPRequest->getLocation()->getRoot();

    std::vector<std::string> allPathsLocation = getAllPathsLoc();
    std::string path = findGoodPath(allPathsLocation);
    if (path.empty()) {
        if (this->HTTPRequest->getLocation())->getAutoindex() == TRUE {
            std::string alias = this->HTTPRequest->getLocation()->getAlias();
            if (!alias.empty()){
                std::string shortPath = HTTPRequest->getPath().substr(HTTPRequest->getLocation()->getPath().size());
                HTTPResponse = listDirectory(alias + shortPath, alias);
            }
            else
                HTTPResponse = listDirectory(root + HTTPRequest->getPath(), root);
            setSate(HTTPResponse::FINISH); 
            return ;
        }
        // return manageNotFound(root + HTTPRequest->getPath());
        std::cout << "location not found !! \n" << std::endl;
     }
     if isLargeFile(path) {
        std::cout << "Incoming Chunked response !!!!" << std::endl;
    //     prepareChunkedResponse(path);
    //     setState(HTTPResponse::CHUNK);
        }
     else{
           std::cout << "Incoming Standard response !!!"  << std::endl;
    //     prepareStandardResponse(path);
    //     setState(HTTPResponse::FINISH);
     }
}


//manageServer

//handle the Get request

void HTTPResponse::handleGetRequest(void)
{
    if  (this->HTTPRequest->getLocation() != NULL)
        manageLocation();
    else 
        std::cout << "Server manager is called!! " std::endl;
        manageServer();
}


//handle the POST request

void HTTPResponse::handlePostRequest(void)
{
    std::string json = "{\n";
    json += "\"message\": \"File uploaded successfuly.\", \n";
    json += "\filename\":  \"" + HTTPRequest->body.getPath() + "\n";
    json +="}\n";

    HTTPResponse = "HTTP/1.1 200 OK\r\n";
    HTTPResponse += "Content-Type: application/json\r\n";
    HTTPResponse +=  "Content-Length: " + std::to_string(json.size()) + "\r\n";
    HTTPResponse += "\r\n";
    HTTPResponse += json;
    this->setState(HTTPResponse::FINISH);
}

//handle the DELETE REQUEST

void HTTPResponse::handleDeleteRequest(void)
{
	std::string path = this->HTTPRequest->location ? this->HTTPRequest->location->getRoot() + this->HTTPRequest->getPath() : this->HTTPRequest->getServer()->getRoot() + this->HTTPRequest->getPath();
	if (!fileExist(path))
		return (this->setError(404));
	if (directoryExist(path.c_str()) || remove(path.c_str()) != 0)
		return (this->setError(403));
	std::string json = "{\n";
	json += "\"message\": \"File deleted successfully.\",\n";
	json += "\"filename\": \"" + HTTPRequest->getPath() + "\"\n";
	json += "}\n";

	HTTPResponse = "HTTP/1.1 200 OK\r\n";
	HTTPResponse += "Content-Type: application/json\r\n";
	HTTPResponse += "Content-Length: " + std::to_string(json.size()) + "\r\n";
	HTTPResponse += "\r\n";
	HTTPResponse += json;
    this->setState(HTTPResponse::FINISH);
}

int HTTPResponse::generateResponse(int pollfd)
{
    (void)pollfd;
    if (!HTTPResponse.empty())
        HTTPResponse.clear();
    if (HTTPRequest->getStateCode() != 200)
        return (this->setError());
    // if (isRedirect())
    //     return (setState())
    // if (this->HTTPRequest->isCgi())
    // {
    //     std::cout << "It's a cgi" << "\n";  
    //     return (this->handleCgi());
    // }
    // std::cout << "It's not a cgi" << "\n";

    if (state != HTTPResponse::CHUNK)
        this->setState(HTTPResponse::PROCESS);
    else if (HTTPRequest->getMethod() == "GET")
        handleGetRequest();
    else if (HTTPRequest->getMethod() == "POST")
        handlePostRequest();
    else if (HTTPRequest->getMethod() == "DELETE")
        handleDeleteRequest();
    else
        return (this->setError(405), 0);
    return (0);
}

//To access to all the state accessor
void HTTPResponse::setState(e_response_state state)
{
    if (this->_state == HTTPResponse::FINISH) {
        std::cout << "[setState (response)] Response already finished" << std::endl;
        return;
    }
    if (this->_state == state) {
        std::cout << "[setState (response)] Response already in this state" << std::endl;
        return;
    }
    this->_state = state;
    switch (this->_state) {
        case HTTPResponse::INIT:
            std::cout << "[setState (response)] Response INIT" << std::endl;
            break;
        case HTTPResponse::PROCESS:
            std::cout << "[setState (response)] Response PROCESS" << std::endl;
            break;
        case HTTPResponse::FINISH:
            std::cout << "[setState (response)] Response FINISH" << std::endl;
            break;
        default:
            std::cout << "[setState (response)] Unknown state" << std::endl;
            break;
    }
}


void HTTPResponse::setError(int code, bool generatePage)
{
    this->HTTPRequest->setStateCode(code);
    if (generatePage)
        this->HTTPResponse = ErrorPage::getPage(code, this->HTTPRequest->getServer()->getErrorPages());
    this->setState(HTTPResponse::FINISH);
}