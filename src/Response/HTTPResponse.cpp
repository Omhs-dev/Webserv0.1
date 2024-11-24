#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"

HTTPResponse::HTTPResponse(Client *client)
{
	_client = client;
	_request = client->getRequest();
	// _server = nullptr;
	_state = INIT;
	_statusCode = "";
	_statusMessage = "";
	_headers = {};
	_body = "";
	_errorPage = "";
	_serverRoot = "./www";
	_fileFd = 0;
}

HTTPResponse::~HTTPResponse()
{
	if (_fileFd > 0)
		close(_fileFd);
}

// --------- Response Generation ---------

void HTTPResponse::generateResponse()
{
	std::string reqMethod = _request->getMethod();
	int reqStateCode = _request->getStateCode();

	if (reqStateCode == 405)
	{
		Logger::ErrorCout("Error Code 405");
		_errorPage = serverErroPage(_client->getRequest()->getStateCode());
		serveFile(_errorPage, "405", getErrorMesssage("405"));
		return;
	}
	if (reqStateCode == 400)
	{
		Logger::ErrorCout("Error Code 400");
		_errorPage = serverErroPage(_client->getRequest()->getStateCode());
		serveFile(_errorPage, "400", getErrorMesssage("400"));
		return;
	}

	if (_request->isCGI())
		handleCGIRequest(*_request);
	else if (reqMethod == "GET")
		handleGet();
	else if (reqMethod == "DELETE")
		handleDelete();
	else if (reqMethod == "POST")
		handlePost();
	else
	{
		Logger::ErrorCout("Error Code 405");
		_errorPage = serverErroPage(_client->getRequest()->getStateCode());
		serveFile(_errorPage, "405", getErrorMesssage("405"));
	}
}

// --------- Handling Requests ---------

void HTTPResponse::handleGet()
{
	std::string reqPath = _request->getPath();
	ServerConfig currentServer = determineServer();
	LocationConfig location = checkLocationPath(reqPath);
	std::string root = _serverRoot;
	std::string indexFilePath = root + reqPath;

	if (reqPath == "/")
	{
		setDefaultResponse(reqPath, location);
	}
	else if (isFile(indexFilePath))
	{
		if(isLargeFile(indexFilePath))
		{
			Logger::Cout("File is large 📄");
			_state = IS_CHUNK;
			setChunkResponse(indexFilePath);
		}
		else
			serveFile(indexFilePath, "200", getErrorMesssage("200"));
	}
	else if (reqPath == location.getLocationPath() && _state == IS_NORMAL)
	{
		cleanPath(reqPath);
		setStandardResponse();
	}
	else if (reqPath == location.getLocationPath() && _state == IS_REDIRECT)
	{
		std::cout << "there might be a redirect here 🔄" << std::endl;
		std::map<int, std::string> redir = location.getRedirect();
		for (auto &red : redir)
		{
			if (red.first == 301)
			{
				setStatus(iToString(red.first), getErrorMesssage(iToString(red.first)));
				break;
			}
		}
		setHeaders("Location", location.getRedirect().begin()->second);
	}
	else if (location.getAlias() != "" && location.getAutoindex() && _state == IS_ALIAS)
	{
		std::string aliasPath = location.getAlias();
		std::string aliasPathIndex = aliasPath + location.getIndex();
		if (isFile(aliasPathIndex))
			serveFile(aliasPathIndex, "200", getErrorMesssage("200"));
		else
			setBody(listDirectory(aliasPath, location.getRoot()));
	}
	else if (_state == IS_NO_LOCATION)
	{
		Logger::NormalCout("no location found");
		_errorPage = serverErroPage(404);
		Logger::Specifique(_errorPage, "error page path");
		serveFile(_errorPage, "404", getErrorMesssage("404"));
	}
	else
	{
		Logger::NormalCout("default error page");
		_errorPage = errorPage(404);
		serveFile(_errorPage, "404", getErrorMesssage("404"));
	}
}

void HTTPResponse::handleDelete()
{
	std::string reqPath = _request->getPath();
	LocationConfig location = checkLocationPath(reqPath);
	std::string serverRooth = _serverRoot;
	std::string reqFilePath = serverRooth + reqPath;
	
	if (reqPath != location.getLocationPath())
	{
		if (!isFile(reqFilePath))
		{
			Logger::ErrorCout("File not found 🚫");
			setStatus("404", getErrorMesssage("404"));
			return;
		}
		if (isDirectory(reqFilePath) || remove(reqFilePath.c_str()) != 0)
			setStatus("403", getErrorMesssage("403"));
		std::string jsonBody = "{\n";
		jsonBody += "  \"message\": \"File deleted successfully.\",\n";
		jsonBody += "  \"filename\": \"" + reqFilePath + "\"\n";
		jsonBody += "}\n";
	
		setBody(jsonBody);
	}
}

void HTTPResponse::handlePost(void)
{
	// std::string responseBody;
	std::string jsonBody = "{\n";
	jsonBody += "\"message\": \"File uploaded successfully.\",\n";
	jsonBody += "\"filename\": \"" + _request->getPath() + "\",\n";
	jsonBody += "\"size\": " + ullToStr(_request->getBody().size()) + "\n";
	jsonBody += "}\n";

	setBody(jsonBody);
}

// --------- Response Types ---------

void HTTPResponse::setChunkResponse(const std::string &path)
{
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
        responseBody << hexSizeStream.str() << "\r\n";
        responseBody.write(buffer, bytesRead);
        responseBody << "\r\n";
        Logger::SpecifiqueForInt(bytesRead, "Chunk size");
    }

    if (bytesRead == -1)
    {
        std::cerr << "Error reading from file: " << path << std::endl;
        setStatus("500", getErrorMesssage("500"));
        setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    }
    else
    {
        responseBody << "0\r\n\r\n";
        setBody(responseBody.str());
        setStatus("200", getErrorMesssage("200"));
    }

    close(_fileFd);
    Logger::NormalCout("Chunked Response Completed");
}

void HTTPResponse::setDefaultResponse(std::string path, LocationConfig config)
{
	std::string indexPath = config.getRoot() + path + config.getIndex();
	serveFile(indexPath, "200", getErrorMesssage("200"));
}

void HTTPResponse::setStandardResponse()
{
	std::string reqPath = _request->getPath();
	LocationConfig location = checkLocationPath(reqPath);
	cleanPath(reqPath);

	std::string fullPath = location.getRoot() + reqPath;
	std::string indexFilePath = fullPath + location.getIndex();

	if (isDirectory(fullPath))
	{
		if (isFile(indexFilePath))
		{
			serveFile(indexFilePath, "200", getErrorMesssage("200"));
			return;
		}
		if (location.autoindex == false)
		{
			Logger::ErrorCout("autoindex off in location");
			_errorPage = serverErroPage(404);
			serveFile(_errorPage, "404", getErrorMesssage("404"));
		}
		else
		{
			std::string directoryListing = listDirectory(reqPath, location.getRoot());
			if (!directoryListing.empty() && _state != IS_ALIAS)
			{
				setStatus("200", getErrorMesssage("200"));
				setBody(directoryListing);
				return;
			}
		}
	}
}

// --------- Engine of the code ---------

ServerConfig HTTPResponse::determineServer()
{
    std::vector<ServerConfig> configs = _client->getServer()->getConfigs();
    auto headers = _client->getRequest()->getHeaders();
    auto it = headers.find("Host");
    if (it == headers.end())
    {
        return ServerConfig();
    }

    std::string hostHeader = it->second;
    std::string hostname, port;
    std::istringstream stream(hostHeader);
    if (std::getline(stream, hostname, ':') && std::getline(stream, port))
    {
        // Logger::Specifique(port, "Extracted port");
    }

	for (std::vector<ServerConfig>::reverse_iterator iter = configs.rbegin(); iter != configs.rend(); ++iter)
    {
		if (std::stoi(port) == std::stoi(iter->_listen))
		{
			_serverRoot = iter->_root;
			return *iter;
		}
    }

    Logger::NormalCout("Server not found! Returning default.");
    return ServerConfig();
}


LocationConfig HTTPResponse::checkLocationPath(const std::string &path)
{
	const auto &server = determineServer();
	for (LocationConfig &location : server.getLocations())
	{
		if (path == location.locationPath && location.redirect.begin()->first > 0
				&& location.redirect.begin()->second != "")
		{
			_state = IS_REDIRECT;
			setServerRoot(server.getRoot());
			return location;
			break;
		}
		else if (path == location.locationPath && location.alias != ""
			&& location.alias != location.locationPath)
		{
			_state = IS_ALIAS;
			setServerRoot(server.getRoot());
			return location;
			break;
		}
		else if (path == location.locationPath)
		{
			_state = IS_NORMAL;
			setServerRoot(server.getRoot());
			return location;
			break;
		}
	}
	Logger::NormalCout("Path is not a Location in server");
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
		Logger::NormalCout("Redirect Response");
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << _headers[0] << "\r\n";
		oss << "Content-Length: 0\r\n";
		oss << "\r\n";
		return oss.str();
	}
	else if (_state == IS_CHUNK)
	{
		Logger::NormalCout("Chunk Response");
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Transfer-Encoding: chunked"  << "\r\n";
		oss << "\r\n";
		oss << _body;
	}
	else
	{
		Logger::NormalCout("Sending Response");
		oss << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";
		oss << "Content-Type: " << getMimeType(_request->getPath()) << "\r\n";
		oss << "Content-Length: " << _body.size() << "\r\n";
		oss << "\r\n";
		oss << _body;
	}
	return oss.str();
}

// --------- Utils Functions ---------

void HTTPResponse::serveFile(const std::string &path, const std::string &code, const std::string &mess)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		setStatus(code, mess);
		setBody(buffer.str());
		file.close();
	}
	else
	{
		Logger::ErrorCout("serving file failed !");
		_errorPage = errorPage(std::stoi(code));
		setStatus(code, mess);
		setBody(_errorPage);
	}
}

void HTTPResponse::cleanPath(std::string &path)
{
	if (path[0] != '/')
		path.insert(0, "/");
	if (path[path.size() - 1] != '/')
		path += "/";

	path.erase(std::unique(path.begin(), path.end(), [](char a, char b)
			{ return a == '/' && b == '/'; }),path.end());

	size_t pos;
	while ((pos = path.find("/./")) != std::string::npos)
	{
		path.erase(pos, 2);
	}

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
	html << "<html>";
	html << "<head>";
	html << "<title>Directory Listing</title>";
	html << "<style>";
	html << "body { font-family: Arial, sans-serif; margin: 20px; }";
	html << "h1 { color: #333; }";
	html << "ul { list-style-type: none; padding: 0; }";
	html << "li { margin: 5px 0;}";
	html << "a:hover { text-decoration: underline; }";
	html << "div { margin-bottom: 10px; }";
	html << "</style>";
	html << "</head>";
	html << "<body>";
	html << "<h1>Directory Listing for " << path << "</h1>";
	html << "<div><a href='../'>" << "../" << "</a></div>";
	html << "<ul>";

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
				if (path.find("uploads") != std::string::npos)
					html << "<li><a href=\"" << path + name << "\" download>" << name << "</a></li>";
				else
					html << "<li><a href=\"" << path + name << "\">" << name << "</a></li>";
			}
		}
	}
	closedir(dir);
	html << "</ul>";
	html << "</body>";
	html << "</html>";
	return html.str();
}

std::string HTTPResponse::getErrorPagePath(int code, ServerConfig server)
{
	std::map<int, std::string>  errorPages = server._errorPage;
	for (auto &page : errorPages)
	{
		if (page.first == code && page.second.find("404.html"))
			return page.second;
		if (page.first == code && page.second.find("403.html"))
			return page.second;
		if (page.first == code && page.second.find("405.html"))
			return page.second;
		if (page.first == code && page.second.find("400.html"))
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
			Logger::Specifique(errorPage, "error page path in serverErrorPages");
			return errorPage;
		}
	}
	return "";
}
