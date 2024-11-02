#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"

HTTPResponse::HTTPResponse(Client *client)
{
	_client = client;
	_server = client->getServer();
	_request = client->getRequest();
	_state = INIT;
	_statusCode = "200";
	_statusMessage = "OK";
	_httpConfigs = new HTTPConfigs();
	_headers = {};
	_body = "";
	// _server = new Server(_httpConfigs);
}

void HTTPResponse::generateResponse()
{
	std::string reqMethod = _request->getMethod();
	
	// std::string goodPath = checkLocationPath(reqPath);
	
	// std::cout << "Good path: " << goodPath << std::endl;
	
	// std::cout << "Request Method: " << reqMethod << std::endl;
	// std::cout << "Request Path at the beginin: " << reqPath << std::endl;
	
	// std::cout << "server root: " << _server->getConfigs()._servers[0].getRoot() << std::endl;
	// std::cout << "location path: " << _server->getConfigs()._servers[0].getLocations()[0].getLocationPath() << std::endl;
	
	if (reqMethod == "GET")
	{
		handleGet();
	}
}

void HTTPResponse::handleGet()
{
	// create a function to check if the path is a file
	// create a function setFileResponse
	// create a function to have server root
	std::string reqPath = _request->getPath();
	LocationConfig location = checkLocationPath(reqPath);
	
	Logger::Separator();
	Logger::VerticalSeparator();
	Logger::Itroduction("handleGet");
	Logger::Specifique(reqPath, "Request Path 🪜 ");
	Logger::Specifique(location.getLocationPath(), "Location Path 🪜 ");
	
	if (isFile("./www" + reqPath))
	{
		// continue implementation from here
		std::cout << "File found 0" << std::endl;
	}
	if (reqPath == "/")
	{
		// checkLocationPath(path);
		setDefaultResponse();
	}
	else if (reqPath == location.getLocationPath() && _state == IS_NORMAL) // will have to make a differece between this and the one below
	{
		std::cout << "    path macthes location path ✔️" << std::endl;
		cleanPath(reqPath);
		setStandardResponse();
		if (isFile(reqPath))
			std::cout << "File found 0" << std::endl;
		std::cout << "reqPath: "  << reqPath << std::endl;
	}
	else if (reqPath == location.getLocationPath() && _state ==IS_REDIRECT)
	{
		bool isRec = false;
	    std::cout << "there might be a redirect here ❎" << isRec << std::endl;
	        
	}
	else
	{
		setStatus("404", "Not Found");
		setBody("<html><body><h1>404 Not Found</h1></body></html>");
	}
}

// bool HTTPResponse::isRedirecUrl(const std::string &path)
// {
	
// }

LocationConfig HTTPResponse::checkLocationPath(const std::string &path)
{
	std::cout << "----- Checking location path ------ : \n" << path << std::endl;
	std::cout << "before for loop" << std::endl;
	for (auto &server : _server->getConfigs()._servers)
	{
		std::cout << "inside for loop 2" << std::endl;
		for (LocationConfig &location : server.getLocations())
		{
			std::cout << "----------------------------------" << std::endl;
			std::cout << "locations path : " << location.getLocationPath() << std::endl;
			// std::cout << "inside for loop 2" << std::endl;
			if (path == location.getLocationPath() && location.getRedirect().begin()->second == "https://github.com/")
			{
				_state = IS_REDIRECT;
				std::cout << "    Redirect found 🔄" << std::endl;
				std::cout << "    Redirect Link found 🔗: " << location.getRedirect().begin()->second << std::endl;
				return location;
				break;
			}
			else if (path == location.getLocationPath())
			{ // Path matches location
				_state = IS_NORMAL;
				std::cout << "      Location found: " << location.getLocationPath() << std::endl;
				return location; // Return the matched location configuration
				break;
			}
			else if (path.find(".html") != std::string::npos) // here to chage .html to isValideFile - check if line has an ext.
			{
				_state = IS_FILE;
				if (path == location.getLocationPath() + location.getIndex())
				{
					std::cout << "      File found 1 📄 :" << location.getLocationPath() + location.getIndex() << std::endl;
					return location;
					break;
				}
			}
		}
		std::cout << "server location index: " << server.getIndex() << std::endl;
		std::cout << "Location not found ❗" << std::endl;
	}
	return LocationConfig();
}

void HTTPResponse::setStandardResponse() {
	// check if the path is a file
		// if yes serve the file
	// check if the file is large
		// if yes prepare chunked response
	// check if the path is a directory
		// check if the directory has an index file
			// if yes serve the index file
		// if not check if the directory has an autoindex on or off
			// if yes serve the directory
	                // else serve page not found 404
    // Clean up the provided path
    std::string reqPath = _request->getPath();
    LocationConfig location = checkLocationPath(reqPath);
    cleanPath(reqPath);
    
    Logger::VerticalSeparator();
	std::cout << "I am in setStandardResponse \n"
				<< "    check again location root: " << location.getRoot() << "\n"
				<< "    check again Request Path🪜: " << reqPath << std::endl;
	
    std::string fullPath = location.getRoot() + reqPath;
    std::string indexFilePath = fullPath + location.getIndex();

	std::cout << "    Index file path int setstandardresponse: " << indexFilePath << std::endl;
	
	
	// std::ifstream file(indexFilePath);

	// if (file.is_open())
	// {
	// 	std::stringstream buffer;
	// 	buffer << file.rdbuf();
	// 	setStatus("200", "OK");
	// 	setBody(buffer.str());
		
	// 	file.close();
	// }
	
	// check if the path is a directory
	if (isFile(reqPath))
		std::cout << "File found 2 in ssr 📄" << std::endl;
	if (isDirectory(fullPath))
	{
		std::cout << "Inside the directory 📁" << std::endl;
		// check if the directory has an index file if yes serve the index file
        std::ifstream indexFile(indexFilePath);
	    if (indexFile.is_open() && indexFilePath.find(".html") != std::string::npos) // here to chage .html to isValideFile
	    {
	        std::cout << "inside index file" << std::endl;
	        std::stringstream buffer;
	        buffer << indexFile.rdbuf(); // Read file contents
	        setStatus("200", "OK");
	        setBody(buffer.str());
	        // reqPath = indexFilePath;
	        indexFile.close();
	        return;
	    }
		// if not check if the directory has an autoindex on or off
	    else if (location.getAutoindex() == 1)
	    {
	        // Generate a directory listing if autoindex is enabled
	        std::cout << "location root in autoindex case: " << location.getRoot() << std::endl;
	        std::cout << "location path in autoindex case: " << location.getLocationPath() << std::endl;
	        std::string directoryListing = listDirectory(reqPath, location.getRoot());
	        if (!directoryListing.empty())
	        {
	            setStatus("200", "OK");
	            setBody(directoryListing);
	            return;
	        }
	    }
	    // else if(reqPath) if reqPath is redirect handle redirect
	}
    else
    {
        // Neither index file found nor autoindex enabled, so return 404
        setStatus("404", "Not Found");
        setBody("<html><body><h1>404 Not Found</h1></body></html>");
    }
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

// bool HTTPResponse::isReqRedirection(const std::string &path)
// {
	
	
		
// }

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
