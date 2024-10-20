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
    std::vector<sdt::string> allPathsLocation;
    std::string path = this->HTTPRequest->getPath();
    std::string root = this->HTTPRequest->getLocation()->getRoot();
    std::string alias = this->HTTPRequest->getLocation()->getAlias();
    std::vector<std::string> indexes = this->HTTPRequest->getLocation()->getIndexes();
    bool isAlias = false;

    if (this->getLocation() == NULL)
        return std::vector<std::string>();
    if (root.empty())
        root = this->HTTPRequest->getServer()->getRoot();
    if (!alias.empty())
        root = alias;
        isAlias = true;
    //in case the request is asking a file direct
    if (path[path.size() - 1] != '/') {
        if (isAlias)
            path = path.substr(this->HTTPRequest->getLocation()->getPath().size());
        allPathLocations.push_back(root + path);
    }

    for (size_t i = 0; i < indexes.size(); i++)
    {
        std::string index = indexes[i];
        std::string bkpPath = path;
        if (path == "/"){
            path = root + "/" + index;
        }
        else if (isAlias){
            path = root + path + index;
        }
        else
            path = root + path + index;
        allPathsLocation.push_back(path);
        path = bkpPath;
    }
    return allPathsLocations;
}

//Checking if the request is looking for a location bloc,
//then manage it
void HTTPResponse::manageLocation()
{
    std::string root;
    this->HTTPRequest->getLocation()->getRoot().empty() ? root = this->HTTPRequest->getServer()->getRoot() : root = this->HTTPRequest->getLocation()->getRoot();

    std::string path = findGoodPath(allPAthsLocation);
    if (path.empty()) {
        if (this->HTTPRequest->getLocation())->getAutoIndex() == TRUE {
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
        return manageNotFound(root + HTTPRequest->getPath());
    }
    if isLargeFile(path) {
        prepareChunkedResponse(path);
        setState(HTTPResponse::CHUNK);
    }
    else{
        prepareStandardResponse(path);
        setState(HTTPResponse::FINISH);
    }

}

//manageServer

//handle the Get request

void HTTPResponse::handleGetRequest(void)
{
    if  (this->HTTPRequest->getLocation() != NULL)
        manageLocation();
    else 
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
    HTTPResponse = "Content-Type: application/json\r\n";
    HTTPResponse =  "Content-Length: " + intToString(json.size()) + "\r\n";
    HTTPResponse = "\r\n";
    HTTPResponse = json;
    this->setState(HTTPResponse::FINISH);
}

//handle the DELETE REQUEST

void Response::handleDeleteRequest(void)
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
	HTTPResponse += "Content-Length: " + intToString(json.size()) + "\r\n";
	HTTPResponse += "\r\n";
	HTTPResponse += json;
	this->setState(Response::FINISH);
}

int HTTPResponse::generateResponse(int pollfd)
{
    (void)pollfd;
    if (!HTTPResponse.empty())
        HTTPResponse.clear();
    if (HTTPRequest->getStateCode() != 200)
        return (this->setError());
    if (isRedirect())
        return (setState())
    if (this->HTTPRequest->isCgi())
    {
        std::cout << "It's a cgi" << "\n";  
        return (this->handleCgi());
    }
    std::cout << "It's not a cgi" << "\n";

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