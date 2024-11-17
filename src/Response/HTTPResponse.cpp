#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"

HTTPResponse::HTTPResponse(Client *client)
{
	_client = client;
	_request = client->getRequest();
	_server = nullptr;
	_state = INIT;
	_statusCode = "200";
	_statusMessage = "OK";
	_headers = {};
	_body = "";
	_errorPage = "";
	_serverRoot = "./www";
}

HTTPResponse::~HTTPResponse()
{
	if (_fileFd > 0)
		close(_fileFd);
}

// --------- Response Generation ---------

void HTTPResponse::generateResponse()
{
	std::cout << "In generateResponse()..\n";
	std::string reqMethod = _request->getMethod();

	if (reqMethod == "GET")
		handleGet();
	else if (reqMethod == "DELETE")
		handleDelete();
	else if (reqMethod == "POST")
		handlePost();
	else
		std::cout << "WTF\n";
}

// --------- Handling Requests ---------

void HTTPResponse::handleGet()
{
	std::string reqPath = _request->getPath();
	std::cout << "Got root\n";
	
	// Logger::SpecifiqueForInt(_client->getServer()->getConfigs().size(), "size of server in handleGet");
	LocationConfig location = checkLocationPath(reqPath);
	std::string root = _serverRoot;
	std::string indexFilePath = root + reqPath;

	std::cout << "In handleGet()..\n";
	// Logger::Separator();
	// Logger::VerticalSeparator();
	// Logger::Itroduction("handleGet");
	// Logger::Specifique(reqPath, "Request Path 🪜 ");
	// Logger::Specifique(location.getLocationPath(), "Location Path 🪜 ");
	// Logger::Specifique(root, "Request Root 🛤️ ");
	// Logger::Specifique(indexFilePath, "Request Root + path + index 🪜 ");
	std::cout << "In handleGet()..\n";
	if (reqPath == "/")
	{
		// checkLocationPath(path);
		setDefaultResponse(reqPath, location);
	}
	else if (isFile(indexFilePath))
	{
		// Logger::Cout("File found here 📄");
		if(isLargeFile(indexFilePath))
		{
			// Logger::Cout("File is large 📄");
			_state = IS_CHUNK;
			setChunkResponse(indexFilePath);
		}
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
			serveFile(aliasPathIndex);
		else
			setBody(listDirectory(aliasPath, location.getRoot()));
	}
	else if (_state == IS_NO_LOCATION)
	{
		// Logger::NormalCout("no location found");
		_errorPage = serverErroPage(404);
		// Logger::Specifique(_errorPage, "error page path");
		setStatus("404", getErrorMesssage("404"));
		serveFile(_errorPage);
	}
	else
	{
		// Logger::NormalCout("default error page");
		_errorPage = errorPage(reqPath, _serverRoot);
		setStatus("404", getErrorMesssage("404"));
		serveFile(_errorPage);
	}
}

void HTTPResponse::handleDelete()
{
	// Logger::Itroduction("handleDelete");
	// Logger::VerticalSeparator();

	std::string reqPath = _request->getPath();

	// Logger::Specifique(reqPath, "Request Path in handleDelete 🪜");
	LocationConfig location = checkLocationPath(reqPath);

	std::string serverRooth = _serverRoot;
	// Logger::Specifique(serverRooth, "serverRooth Root 🛤️");

	std::string reqFilePath = serverRooth + reqPath;
	// Logger::Specifique(reqFilePath, "Request File Path 🪜");
	
	if (reqPath != location.getLocationPath())
	{
		// Logger::Cout("Path not found 🚫");

		if (!isFile(reqFilePath))
		{
			// Logger::Cout("File not found 🚫");
			setStatus("404", getErrorMesssage("404"));
			return;
		}
		// Logger::Cout("File found here 📄");
		if (isDirectory(reqFilePath) || remove(reqFilePath.c_str()) != 0)
			setStatus("403", getErrorMesssage("403"));
		std::string jsonBody = "{\n";
		jsonBody += "  \"message\": \"File deleted successfully.\",\n";
		jsonBody += "  \"filename\": \"" + reqFilePath + "\"\n";
		jsonBody += "}\n";
	
		// Logger::Specifique(jsonBody, "Json Body 🪜");
		setBody(jsonBody);
	}
}

void HTTPResponse::handlePost(void)
{
	// std::string responseBody;
	std::string jsonBody = "{\n";
	jsonBody += "\"message\": \"File uploaded successfully.\",\n";
	jsonBody += "\"filename\": \"" + _request->getPath() + "\",\n";
	jsonBody += "\"size\": " + ullToStr(_body.size()) + "\n";
	jsonBody += "}\n";

	setBody(jsonBody);
}

// --------- Response Types ---------

void HTTPResponse::setChunkResponse(const std::string &path)
{
    // Logger::Itroduction("setChunkResponse");
    // Logger::NormalCout("Starting Chunked Response");
    _fileFd = open(path.c_str(), O_RDONLY);
    if (_fileFd == -1)
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        setStatus("404", "Not Found");
        setBody("<html><body><h1>404 Not Found</h1></body></html>");
        return;
    }

    std::ostringstream responseBody;
    char buffer[MAX_FILE_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(_fileFd, buffer, MAX_FILE_SIZE)) > 0)
    {
        std::stringstream hexSizeStream;
        hexSizeStream << std::hex << bytesRead;
        responseBody << hexSizeStream.str() << "\r\n";  // Write the chunk size in hex followed by CRLF
        responseBody.write(buffer, bytesRead);          // Write the actual chunk data
        responseBody << "\r\n";                         // End the chunk with CRLF
        std::cout << "Chunk size: " << bytesRead << std::endl;
    }

    if (bytesRead == -1)
    {
        std::cerr << "Error reading from file: " << path << std::endl;
        setStatus("500", "Internal Server Error");
        setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    }
    else
    {
        responseBody << "0\r\n\r\n";
        setBody(responseBody.str());
        setStatus("200", "OK");
    }

    close(_fileFd);
    // Logger::NormalCout("Chunked Response Completed");
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

// --------- Engine of the code ---------

LocationConfig HTTPResponse::checkLocationPath(const std::string &path)
{
	// Logger::NormalCout("-------------- checkLocationPath --------------");
	// Logger::Specifique(path, "Request Path 🪜");
	// Logger::NormalCout("before for loop 1 \n|");
	std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	for (auto &server : configs)
	{
			// Logger::NormalCout("before for loop 2\n|");
			// Logger::NormalCout("Liste of locations ../../ ⬇");
			// Logger::NormalCout("|");
			// Logger::Specifique(server._root, "server root here");
			// Logger::SpecifiqueForInt(server._locations.size(), "location size in the checklocationpath function");
		for (LocationConfig &location : server.getLocations())
		{
			// Logger::Separator();
			// Logger::Specifique(location.getLocationPath(), "Location Path to look for 🪜");
			// if (path == location.locationPath)
				Logger::NormalCout("yes...");
			// if (location.redirect.begin()->second.find("github"))
				Logger::NormalCout("github redirect found here");
			if (path == location.locationPath && location.redirect.begin()->first > 0
					&& location.redirect.begin()->second != "")
			{
				// Logger::NormalCout("in redirection ");
				_state = IS_REDIRECT;
				// Logger::NormalCout("Redirect found 🔄");
				// Logger::Specifique(location.getRedirect().begin()->second, "Redirect Link found 🔗");
				setServerRoot(server.getRoot());
				// Logger::Specifique(_serverRoot, "Server root set successfully..");
				return location;
				break;
			}
			else if (path == location.locationPath && location.alias != ""
						&& location.alias != location.locationPath)
			{
				_state = IS_ALIAS;
				// Logger::Specifique(location.getLocationPath(), "Location Path 🪜");
				// Logger::Specifique(location.getAlias(), "Alias found 🪜");
				// Logger::Specifique(location.getAlias(), "Alias path 🪜");
				setServerRoot(server.getRoot());
				// Logger::Specifique(_serverRoot, "Server root set successfully..");
				return location;
				break;
			}
			else if (path == location.locationPath)
			{
				// Logger::NormalCout("Location found ✅");
				_state = IS_NORMAL;
				setServerRoot(server.getRoot());
				// Logger::Specifique(_serverRoot, "Server root set successfully..");
				return location;
				break;
			}
			// Logger::NormalCout("Location not found ❗");
		}
		// std::cout << "server location index: " << server.getIndex() << std::endl;
		// Logger::NormalCout("|\nNext server 🚀");
	}
	_state = IS_NO_LOCATION;
	return LocationConfig();
}

// --------- Response Setter ---------

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

void HTTPResponse::setServerRoot(const std::string &root)
{
	_serverRoot = root;
}


// --------- Response Datas ---------

std::string HTTPResponse::getData() const
{
	// Logger::Itroduction("getData 📊");
	
	std::ostringstream oss;
	if (_state == IS_REDIRECT)
	{
		std::cout << "HERE 1\n";
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << _headers[0] << "\r\n";
		oss << "Content-Length: 0\r\n";
		oss << "\r\n";
		return oss.str();
	}
	else if (_state == IS_CHUNK)
	{
		std::cout << "HERE 2\n";
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Transfer-Encoding: chunked"  << "\r\n";
		oss << "\r\n";
		oss << _body;
	}
	else
	{
		std::cout << "HERE 3\n";
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Content-Length: " << _body.size() << "\r\n";
		oss << "\r\n";
		oss << _body;
	}
	// std::cout << oss.str();
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
}

std::string HTTPResponse::getErrorPagePath(int code, ServerConfig server)
{
	std::map<int, std::string>  errorPages = server._errorPage;
	for (auto &page : errorPages)
	{
	// Logger::SpecifiqueForInt(page.first, "status code");
		if (page.first == code && page.second.find("404.html"))
			return page.second;
		if (page.first == code && page.second.find("403.html"))
			return page.second;
	}
	return "";
}

std::string HTTPResponse::serverErroPage(int code)
{
	std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
	for (ServerConfig &server : configs)
	{
		std::string errorPage = getErrorPagePath(code, server);
		if (!errorPage.empty())
		{
			// Logger::Specifique(errorPage, "error page path in serverErrorPages");
			return errorPage;
		}
		// Logger::NormalCout("path empty");
	}
	return "";
}
