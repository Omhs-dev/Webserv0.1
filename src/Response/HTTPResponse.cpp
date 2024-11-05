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
	_headers = {};
	_body = "";
	_errorPage = "";
}

// --------- Response Generation ---------

void HTTPResponse::generateResponse()
{
	std::string reqMethod = _request->getMethod();

	if (reqMethod == "GET")
	{
		handleGet();
	}
}

// --------- Handling Requests ---------

void HTTPResponse::handleGet()
{
	std::string reqPath = _request->getPath();
	std::string reqRooth = _server->getConfigs()._servers[0].getRoot();
	std::string indexFilePath = reqRooth + reqPath;
	LocationConfig location = checkLocationPath(reqPath);

	// Logger::Separator();
	// Logger::VerticalSeparator();
	// Logger::Itroduction("handleGet");
	// Logger::Specifique(reqPath, "Request Path 🪜 ");
	// Logger::Specifique(location.getLocationPath(), "Location Path 🪜 ");
	// Logger::Specifique(reqRooth, "Request Root 🛤️ ");
	// Logger::Specifique(indexFilePath, "Request Root + path + index 🪜 ");

	if (reqPath == "/")
	{
		// checkLocationPath(path);
		setDefaultResponse(reqPath, location);
	}
	else if (isFile(indexFilePath))
	{
<<<<<<< HEAD
		// if (isPathValid(path))
		// 	std::cout << "Checking if Path is valid" << std::endl;
		setStandardResponse("./www" + path);
		std::cout << "Path: " << path << std::endl;
=======
		// Logger::Cout("File found here 📄");
		if(isLargeFile(indexFilePath) && _state == IS_CHUNK)
			setChunkResponse(indexFilePath);
		else
			serveFile(indexFilePath);
	}
	else if (reqPath == location.getLocationPath() && _state == IS_NORMAL)
	{
		// Logger::Cout("Path matches location path here ✅");
		cleanPath(reqPath);
		setStandardResponse();
		// Logger::Specifique(reqPath, "Request Path 🪜");
	}
	else if (reqPath == location.getLocationPath() && _state == IS_REDIRECT)
	{
		std::cout << "there might be a redirect here 🔄" << std::endl;
		std::map<int, std::string> redir = location.getRedirect();
		for (auto &red : redir)
		{
			if (red.first == 301)
			{
				// Logger::Cout("301 Redirect found 🔄");
				setStatus(iToString(red.first), getErrorMesssage(iToString(red.first)));
				break;
			}
		}
		// Logger::Checker(location.getRedirect().begin()->second);
		setHeaders("Location", location.getRedirect().begin()->second);
	}
	else if (location.getAlias() != "" && location.getAutoindex() && _state == IS_ALIAS)
	{
		// Logger::Cout("Alias found 🪜");
		// Logger::Specifique(location.getAlias(), "Here is the Alias 🪜 :");
		std::string aliasPath = location.getAlias();
		std::string aliasPathIndex = aliasPath + location.getIndex();
		// Logger::Specifique(aliasPath, "Alias Path 🪜");
		// Logger::Specifique(aliasPathIndex, "aliasPathIndex Path 🪜");
		if (isFile(aliasPathIndex))
		{
			if (isLargeFile(aliasPathIndex))
				setChunkResponse(aliasPathIndex);
			else
				serveFile(aliasPathIndex);
		}
		else
			setBody(listDirectory(aliasPath, location.getRoot()));
>>>>>>> b1db622744670f71aa8edfe574af9dd3c50fa0f8
	}
	else
	{
		_errorPage = errorPage(reqPath, reqRooth);
		setStatus("404", "Not Found");
		setBody(_errorPage);
	}
}


//setting the chunkResponse
void HTTPResponse::setChunkResponse(const std::string &path)
{
	std::cout << "here is Chunk response reading!!" << std::endl;
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        _errorPage = errorPage(path, "./www/");
        setStatus("404", "Not Found");
        setBody(_errorPage);
        return;
    }
    setStatus("200", "OK");

    std::ostringstream responseBody;
    // Read the file in chunks
    char buffer[MAX_RESPONSE_BODY_SIZE]; 
    while (file)
    {
        file.read(buffer, sizeof(buffer));
        std::streamsize bytesRead = file.gcount(); // Get number of bytes read

        if (bytesRead > 0)
        {
            // Convert bytesRead to hexadecimal
            std::string chunkSize = intToHexa(bytesRead) + "\r\n";
            std::string chunkData(buffer, bytesRead);
            responseBody << chunkSize << chunkData << "\r\n"; // Append chunk to response body
        }
    }
    responseBody << "0\r\n\r\n"; // Final chunk to signal end
    file.close();
		std::cout << "here is Chunk response endingggggggggg!!" << std::endl;

    setBody(responseBody.str());

}


void HTTPResponse::setDefaultResponse(std::string path, LocationConfig config)
{
	std::string indexPath = config.getRoot() + path + config.getIndex();

	// Logger::Itroduction("setDefaultResponse");
	// Logger::Specifique(config.getRoot() + path + config.getIndex(), "Index file path 📄");

	serveFile(indexPath);
}

void HTTPResponse::setStandardResponse()
{
	std::string reqPath = _request->getPath();
	LocationConfig location = checkLocationPath(reqPath);
	cleanPath(reqPath);

	// Logger::VerticalSeparator();
	// Logger::Itroduction("setStandardResponse");

	std::string fullPath = location.getRoot() + reqPath;
	std::string indexFilePath = fullPath + location.getIndex();

	// Logger::Specifique(reqPath, "Request Path 🪜");
	// Logger::Specifique(fullPath, "FullPath here 🪜");
	// Logger::Specifique(indexFilePath, "Index File Path 🪜");

	if (isDirectory(fullPath))
	{
		// Logger::Cout("Directory found 📁");
		// Logger::Cout("Checking for index file or autoindex 📁");
		// Logger::SpecifiqueForBool(location.getAutoindex(), "Autoindex 🪜  ");
		// Logger::Specifique(location.getAlias(), "Alias 🪜");
		// Logger::Specifique(location.getRoot(), "Root 🪜");

		// check if the directory has an index file if yes serve the index file
		if (isFile(indexFilePath))
		{
			// Logger::Cout("Index file found 📄");
			serveFile(indexFilePath);
			return;
		}
		// if not check if the directory has an autoindex on or off
		else if (location.getAutoindex() == true)
		{
			// Logger::Cout("Autoindex found 📁");
			// Logger::Specifique(reqPath, "Request Path 🪜");
			// Logger::Specifique(location.getRoot(), "Root 🪜");

			std::string directoryListing = listDirectory(reqPath, location.getRoot());
			if (!directoryListing.empty() && _state != IS_ALIAS)
			{
				setStatus("200", "OK");
				setBody(directoryListing);
				return;
			}
		}
	}
	else
	{
		_errorPage = errorPage(reqPath, location.getRoot());
		setStatus("404", "Not Found");
		setBody(_errorPage);
	}
}

// --------- Motor of the code ---------

LocationConfig HTTPResponse::checkLocationPath(const std::string &path)
{
	// Logger::NormalCout("-------------- checkLocationPath --------------");
	// Logger::Specifique(path, "Request Path 🪜");
	// Logger::NormalCout("before for loop 1 \n|");
	for (auto &server : _server->getConfigs()._servers)
	{
			// Logger::NormalCout("before for loop 2\n|");
			// Logger::NormalCout("Liste of locations ../../ ⬇");
			// Logger::NormalCout("|");
		for (LocationConfig &location : server.getLocations())
		{
			// Logger::Separator();
			// Logger::Specifique(location.getLocationPath(), "Location Path to look for 🪜");
			if (path == location.getLocationPath()
					&& location.getRedirect().begin()->second == "https://github.com/")
			{
				_state = IS_REDIRECT;
				// Logger::NormalCout("Redirect found 🔄");
				// Logger::Specifique(location.getRedirect().begin()->second, "Redirect Link found 🔗");
				return location;
				break;
			}
			else if (path == location.getLocationPath() && location.getAlias() != ""
						&& location.getAlias() != location.getLocationPath())
			{
				_state = IS_ALIAS;
				// Logger::Specifique(location.getLocationPath(), "Location Path 🪜");
				// Logger::Specifique(location.getAlias(), "Alias found 🪜");
				// Logger::Specifique(location.getAlias(), "Alias path 🪜");
				return location;
				break;
			}
			else if (path == location.getLocationPath())
			{
				// Logger::NormalCout("Location found ✅");
				_state = IS_NORMAL;
				return location;
				break;
			}
			else if (path.find(".html") != std::string::npos) // here to chage .html to isValideFile - check if line has an ext.
			{
				if (path == location.getLocationPath() + location.getIndex())
				{
					// Logger::NormalCout("File found here 📄");
					_state = IS_FILE;
					return location;
					break;
				}
			}
			// Logger::NormalCout("Location not found ❗");
		}
		// std::cout << "server location index: " << server.getIndex() << std::endl;
		// Logger::NormalCout("|\nNext server 🚀");
	}
	return LocationConfig();
}

<<<<<<< HEAD
=======
// --------- Response Setter ---------

>>>>>>> b1db622744670f71aa8edfe574af9dd3c50fa0f8
void HTTPResponse::setStatus(const std::string &code, const std::string &message)
{
	_statusCode = code;
	_statusMessage = message;
}

void HTTPResponse::setHeaders(const std::string &key, const std::string &value)
{
	_headers.push_back(key + ": " + value);
}

void HTTPResponse::setBody(const std::string &body)
{
	_body = body;
}

// --------- Response Datas ---------

std::string HTTPResponse::getData() const
{
	std::ostringstream oss;
<<<<<<< HEAD
	oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << _body.size() << "\r\n";
	oss << "\r\n";
	oss << _body;
	return oss.str();
}

// --- Checkers ---

bool HTTPResponse::isFile(const std::string &path)
{
	struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

bool HTTPResponse::isFileLarge(const std::string &path)
{
	std::ifstream file("./www" + path);
	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.close();
	return fileSize > MAX_ALLOWED_FILE_SIZE;
}

bool HTTPResponse::isDirectory(const std::string &path)
{
	struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool HTTPResponse::isPathValid(const std::string &path)
{
    // Check if _client is not null
    if (!_client) {
        std::cerr << "Error: _client is null" << std::endl;
        return false;
    }

    // Check if _client->getServer() is not null
    if (!_client->getServer()) {
        std::cerr << "Error: Server pointer in _client is null" << std::endl;
        return false;
    }

    // Check if _client->getServer()->getConfigs() is not null
    const HTTPConfigs& configs = _client->getServer()->getConfigs();
    if (configs._servers.empty()) {
        std::cerr << "Error: Server configs are empty" << std::endl;
        return false;
    }

    // Loop through the server configurations
    for (const ServerConfig& serverConfig : configs._servers)
    {
        std::cout << "Server root: " << serverConfig._root << std::endl;
        if (path.find(serverConfig._root) != std::string::npos)
        {
            std::cout << "Path is valid" << std::endl;
            return true;
        }
        std::cout << "Path is not valid" << std::endl;
    }

    return false;
=======
	if (_state == IS_REDIRECT)
	{
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << _headers[0] << "\r\n";
		oss << "Content-Length: 0\r\n";
		oss << "\r\n";
		return oss.str();
	}
	else if (_state == IS_CHUNK)
	{
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Content-Length: " << _body.size() << "\r\n";
		oss << "Transfer-Encoding: chunked"  << "r\n";
		oss << "\r\n";
		oss << _body;
	}
	else
	{
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Content-Length: " << _body.size() << "\r\n";
		oss << "\r\n";
		oss << _body;
	}
	return oss.str();
}

// --------- Utils Functions ---------

void HTTPResponse::serveFile(const std::string &path)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		setStatus("200", "OK");
		setBody(buffer.str());
		file.close();
	}
	else
	{
		_errorPage = errorPage(path, "/www/");
		setStatus("404", "Not Found");
		setBody(_errorPage);
	}
}

void HTTPResponse::cleanPath(std::string &path)
{
	// Ensure path starts with '/' and ends with '/'
	if (path[0] != '/')
		path.insert(0, "/");
	if (path[path.size() - 1] != '/')
		path += "/";

	// Normalize multiple slashes to a single slash
	path.erase(std::unique(path.begin(), path.end(), [](char a, char b)
			{ return a == '/' && b == '/'; }),path.end());

	// Remove all "/./" occurrences
	size_t pos;
	while ((pos = path.find("/./")) != std::string::npos)
	{
		path.erase(pos, 2);
	}

	// Remove all "/prev/../" occurrences
	while ((pos = path.find("/../")) != std::string::npos)
	{
		if (pos == 0)
		{
			path.erase(0, 3);
			continue;
		}
		size_t prev = path.rfind('/', pos - 1);
		if (prev != std::string::npos)
		{
			path.erase(prev, pos - prev + 3);
		}
		else
		{
			path.erase(0, pos + 3);
		}
	}
}


    //  The variable DIR* is a pointer to a DIR structure,
	//  which represents a directory stream in C and C++ 
	//  programs that interact with directories using POSIX 
	//  directory-handling functions (available in <dirent.h>).
	//  When you use functions like opendir, readdir, and closedir,
	//  you work with DIR* to manage and access the contents of a directory.

std::string HTTPResponse::listDirectory(const std::string &path, const std::string &root)
{
	// Logger::Itroduction("listDirectory 📁 📂");
	std::string fullPath = root + path;
	DIR *dir = opendir(fullPath.c_str());
	if (!dir)
	{
		return "";
	}
	std::stringstream html;
	html << "<html><head><title>Directory Listing</title></head><body>";
	html << "<h1>Directory Listing for " << path << "</h1><ul>";

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;

		struct stat info;
		std::string itemPath = fullPath + "/" + name;
		if (stat(itemPath.c_str(), &info) == 0)
		{
			if (S_ISDIR(info.st_mode))
			{
				html << "<li><b><a href=\"" << path + name + "/" << "\">" << name << "/</a></b></li>";
			}
			else
			{
				html << "<li><a href=\"" << path + name << "\">" << name << "</a></li>";
			}
		}
	}
	closedir(dir);
	html << "</ul></body></html>";
	return html.str();
>>>>>>> b1db622744670f71aa8edfe574af9dd3c50fa0f8
}
