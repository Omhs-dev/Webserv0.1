// ConfigParser.cpp

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}
ConfigParser::~ConfigParser() {}

HTTPConfigs ConfigParser::getHTTPConfigs()
{
    return this->httpConfigs;
}

void ConfigParser::parseConfigFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#' || line == "{")
            continue;

        if (line == "http {")
        {
            removeWhiteSpace(line);
            parseHttpBlock(file);
        }
        else
        {
            throw std::runtime_error("Unexpected line: " + line);
        }
    }

    file.close();
}

void ConfigParser::initConfigServer(ServerConfig &serverConf)
{
    serverConf._clientMaxBodySize = "100";

    this->required.push_back("index");
    this->required.push_back("root");
    this->required.push_back("server_name");
    this->required.push_back("listen");
    this->required.push_back("autoindex");
    this->required.push_back("allow_methods");
    this->required.push_back("max_body_size");
    this->required.push_back("error_page");
    this->required.push_back("return");
    this->required.push_back("location");
    this->defined.clear();
}

void ConfigParser::parseHttpBlock(std::ifstream &file)
{
    this->blockStack.push_back(HTTP);

    std::string line;
    while (!blockStack.empty() && std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line == "{")
            continue;
        if (line == "server {")
        {
            ServerConfig serverConfig;
            initConfigServer(serverConfig);
            parseServerBlock(file, serverConfig);
            this->httpConfigs._servers.push_back(serverConfig);
        }
        else if (line == "}")
        {
            blockStack.pop_back();
        }
        else
        {
            throw std::runtime_error("Unexpected line inside http block: " + line);
        }
    }
}

void ConfigParser::parseServerBlock(std::ifstream &file, ServerConfig &serverConfig)
{
    this->blockStack.push_back(SERVER);

    std::string line;
    while (!this->blockStack.empty() && std::getline(file, line))
    {
        removeWhiteSpace(line);
        size_t found = line.find("location");
        if (line.empty() || line[0] == '#' || line == "{")
            continue;

        if (line.find("}") != std::string::npos)
        {
            this->blockStack.pop_back();
            break;
        }
        else if (found != std::string::npos)
        {
            ConfigLocation confLoc;
            locationPath(line, confLoc);
            parseLocationBlock(file, confLoc);
            serverConfig._locations.push_back(confLoc);
        }
        else
        {
            std::string key, value;
            setKeyValue(line, key, value);

            if (key == "listen")
            {
                size_t found = value.find(":");
                if (found != std::string::npos)
                {
                    std::string serverName = value.substr(0, found);
                    std::string port = value.substr(found + 1);
                    serverConfig._listen = serverName;
                    serverConfig._port = port;
                }
                else
                {
                    serverConfig._listen = value;
                }
            }
            else if (key == "server_name")
            {
                serverConfig._serverName = value;
            }
            else if (key == "root")
            {
                serverConfig._root = value;
            }
            else if (key == "autoindex")
			{
				if (value == "on")
				{
					serverConfig._autoindex = true;
				}
				else if (value == "off")
				{
					serverConfig._autoindex = false;
				}
				else
				{
					throw std::runtime_error("Invalid value for autoindex: " + value);
				}
			}
            else if (key == "index")
            {
                serverConfig._index = value;
            }
            else if (key == "max_body_size")
            {
                serverConfig._clientMaxBodySize = value;
            }
            else if (key == "error_page")
            {
                size_t pos = value.find(" ");
                if (pos != std::string::npos)
                {
                    int code = std::stoi(value.substr(0, pos));
                    std::string path = value.substr(pos + 1);
                    serverConfig.errorPage[code] = path;
                }
            }
            else if (key == "allow_methods")
            {
                std::istringstream iss(value);
                std::string method;
                while (iss >> method)
                {
                    RequestType type = strToRequestType(trim(method));
                    serverConfig._requestAllowed.push_back(type);
                }
            }
            else if (key == "return")
            {
                serverConfig._redirect = value;
            }
            else
            {
                throw std::runtime_error("Unknown directive in server block: " + key);
            }

            this->defined.push_back(key);
            this->required.erase(std::remove(this->required.begin(), this->required.end(), key), this->required.end());
        }
    }
}

void ConfigParser::parseLocationBlock(std::ifstream &file, ConfigLocation &confLocation)
{
    this->blockStack.push_back(LOCATION);

    std::string line;
    std::string key, value;
    while (!this->blockStack.empty() && std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        if (line == "}")
        {
            this->blockStack.pop_back();
            break;
        }
        else
        {
            setKeyValue(line, key, value);

            if (key == "allow_methods")
            {
                std::istringstream iss(value);
                std::string method;
                while (iss >> method)
                {
                    RequestType type = strToRequestType(trim(method));
                    confLocation.requestAllowed.push_back(type);
                }
            }
            else if (key == "root")
            {
                confLocation.root = value;
            }
            else if (key == "autoindex")
            {
                if (value == "on")
				{
					confLocation.autoindex = true;
				}
				else if (value == "off")
				{
					confLocation.autoindex = false;
				}
				else
				{
					throw std::runtime_error("Invalid value for autoindex: " + value);
				}
            }
            else if (key == "alias")
            {
                confLocation.alias = value;
            }
            else if (key == "index")
            {
                confLocation.index = value;
            }
            else if (key == "cgi")
            {
                std::istringstream iss(value);
                std::string extension, path;
                if (iss >> extension >> path)
                {
                    confLocation.cgi[extension] = path;
                }
            }
            else if (key == "error_page")
            {
                size_t pos = value.find(" ");
                if (pos != std::string::npos)
                {
                    int code = std::stoi(value.substr(0, pos));
                    std::string path = value.substr(pos + 1);
                    confLocation.errorPage[code] = path;
                }
            }
            else if (key == "max_body_size")
            {
                confLocation.clientMaxBodySize = value;
            }
            else if (key == "return")
            {
                confLocation.redirect = value;
            }
            else
            {
                throw std::runtime_error("Unknown directive in location block: " + key);
            }
        }
    }
}

void ConfigParser::locationPath(const std::string &line, ConfigLocation &confLocation)
{
    size_t pos = line.find('{');
    if (pos != std::string::npos)
    {
        confLocation.locationPath = line.substr(0, pos);
        removeWhiteSpace(confLocation.locationPath);
    }
    else
    {
        throw std::runtime_error("Invalid location line: " + line);
    }
}p

// ----------------- Utils ----------------- //

void ConfigParser::setKeyValue(std::string &line, std::string &key, std::string &value)
{
    key = "";
    value = "";

    line = trim(line);

    size_t keyEnd = line.find_first_of(" \t");
    if (keyEnd == std::string::npos) {
        key = line; // The entire line is the key
    } else {
        key = line.substr(0, keyEnd);
        size_t valueStart = line.find_first_not_of(" \t", keyEnd);
        if (valueStart != std::string::npos) {
            value = line.substr(valueStart);
        }
    }

    key = trim(key);
    value = trim(value);
}

void ConfigParser::removeWhiteSpace(std::string& line)
{
    const std::string whiteSpace = " \n\r\t\f\v";
    size_t start = line.find_first_not_of(whiteSpace);
    size_t end = line.find_last_not_of(whiteSpace);
    if (start == std::string::npos || end == std::string::npos) {
        line.clear(); // If line is entirely whitespace, clear it
    } else {
        line = line.substr(start, end - start + 1);
    }
}

std::string ConfigParser::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}


std::string ConfigParser::toUpperCase(const std::string& str)
{
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

RequestType ConfigParser::strToRequestType(const std::string& str)
{
    std::string trimmedStr = trim(str);
    std::string upperStr = toUpperCase(trimmedStr);
    
    if (upperStr == "GET")
        return GET;
    if (upperStr == "POST")
        return POST;
    if (upperStr == "DELETE")
        return DELETE;
    throw std::runtime_error("Unsupported request: " + str);
}

void ConfigParser::printRequestTypes(const std::vector<RequestType>& requestTypes)
{
    for (std::vector<RequestType>::const_iterator it = requestTypes.begin(); it != requestTypes.end(); ++it)
    {
        switch (*it) {
            case GET:
                std::cout << "GET ";
                break;
            case POST:
                std::cout << "POST ";
                break;
            case DELETE:
                std::cout << "DELETE ";
                break;
        }
    }
    std::cout << std::endl;
}

void ConfigParser::printConfigLocation(const ConfigLocation& location)
{
    std::cout << "  Location Path: " << location.locationPath << std::endl;
    std::cout << "  Allowed Methods: ";
    printRequestTypes(location.requestAllowed);
    if (!location.root.empty()) {
        std::cout << "  Root: " << location.root << std::endl;
    }
    std::cout << "  Autoindex: " << (location.autoindex ? "on" : "off") << std::endl;
    if (!location.alias.empty()) {
        std::cout << "  Alias: " << location.alias << std::endl;
    }
    if (!location.index.empty()) {
        std::cout << "  Index: " << location.index << std::endl;
    }
    if (!location.cgi.empty()) {
        std::cout << "  CGI: ";
        for (std::map<std::string, std::string>::const_iterator it = location.cgi.begin(); it != location.cgi.end(); ++it) {
            std::cout << it->first << " -> " << it->second << ", ";
        }
        std::cout << std::endl;
    }
    if (!location.redirect.empty()) {
        std::cout << "  Redirect: " << location.redirect << std::endl;
    }
}

void ConfigParser::printServerConfig(const ServerConfig& server)
{
    std::cout << "Server {" << std::endl;
    std::cout << "  Listen: " << server._listen << std::endl;
    std::cout << "  Server Name: " << server._serverName << std::endl;
    std::cout << "  Root: " << server._root << std::endl;
    std::cout << "  Index: " << server._index << std::endl;
    std::cout << "  Max Body Size: " << server._clientMaxBodySize << std::endl;
    for (std::map<int, std::string>::const_iterator it = server.errorPage.begin(); it != server.errorPage.end(); ++it) {
    std::cout << "  Error Page: " << it->first << " -> " << it->second << std::endl;
    }
    for (std::vector<ConfigLocation>::const_iterator it = server._locations.begin(); it != server._locations.end(); ++it) {
        std::cout << "  Location {" << std::endl;
        printConfigLocation(*it);
        std::cout << "  }" << std::endl;
    }
    std::cout << "}" << std::endl;
}
