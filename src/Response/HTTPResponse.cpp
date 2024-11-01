#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"


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
		setDefaultResponse();
	else if (path != "/")
	{
        LocationConfig item = checkLocationPath(path);
        std::cout <<"item location path and other: "<< item.getRoot() << std::endl;
		setStandardResponse(path);
		std::cout << "Path: " << path << std::endl;

	}
	else if (path == "/test")
	{
		setStandardResponse(path);
		std::cout << "Path: "  << path << std::endl;
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

	// std::string root;
	// if(this->_request->getLocation()->getRoot().empty())
	// 	root = this-> _request->getServer()->getRoot();
	// root = this->_request->getLocation()->getRoot();
	

	// std::cout << "Path not found" << std::endl;

// void HTTPResponse::setStandardResponse(const std::string &path)
// {

// 	if(path.empty())
// 	std::string response;
// 	response = listDirectory("./www", path);
// 	std::ifstream file(path);

// 	if (file.is_open())
// 	{
// 		std::stringstream buffer;
// 		buffer << file.rdbuf();
// 		setStatus("200", "OK");
// 		setBody(buffer.str());
		
// 		file.close();
// 	}
// 	else
// 	{
// 		setStatus("404", "Not Found");
// 		setBody("<html><body><h1>404 Not Found</h1></body></html>");
// 	}
// }


void HTTPResponse::setStandardResponse(const std::string& path) {
    // Clean up the provided path
    std::string cleanPathStr = path;
    cleanPath(cleanPathStr);

    // Construct the full path by prepending the root directory, e.g., "./www"
    std::string fullPath = "./www" + cleanPathStr;

    // Check if an index file exists in the directory (e.g., "index.html")
    std::string indexFilePath = fullPath + "index.html";
    std::ifstream indexFile(indexFilePath);

	std::cout << "Index file path int setstandardresponse: " << indexFilePath << std::endl;

    // Serve the index file if it exists
    if (indexFile.is_open()) {
        std::stringstream buffer;
        buffer << indexFile.rdbuf(); // Read file contents
        setStatus("200", "OK");
        setBody(buffer.str());
        indexFile.close();
        return;
    }
	bool _autoIndex = 1;
    // No index file found, so check autoindex status
    if (_autoIndex){
        // Generate a directory listing if autoindex is enabled
        std::string directoryListing = listDirectory(cleanPathStr, "./www");
        if (!directoryListing.empty()) {
            setStatus("200", "OK");
            setBody(directoryListing);
            return;
        }
    }
    // Neither index file found nor autoindex enabled, so return 404
    setStatus("404", "Not Found");
    setBody("<html><body><h1>404 Not Found Narcisse</h1></body></html>");
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

void HTTPResponse::cleanPath(std::string &path) {
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

// Lists the contents of a directory and returns an HTML-formatted string
std::string HTTPResponse::listDirectory(const std::string& path, const std::string& root) {
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
