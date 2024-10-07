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
            // removeWhiteSpace(line);
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
        std::string::size_type found = line.find("location");
        // printf("line: %s\n", line.c_str());
        if (line.empty() || line[0] == '#' || line == "{")
            continue;

        if (line.find("}") != std::string::npos)
        {
            this->blockStack.pop_back();
            break;
        }
        else if (found != std::string::npos)
        {
            // std::cout << "location found" << line << std::endl;
            ConfigLocation confLoc;
            locationPath(line, confLoc);
            parseLocationBlock(file, confLoc);
            serverConfig._locations.push_back(confLoc);
        }
        else
        {
            // printf("location path: %s\n", line.c_str());
            std::string key, value;
            setKeyValue(line, key, value);

            if (key == "listen")
            {
                /* split by : to have the server name and port */
                std::string::size_type found = value.find(":");
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
                    serverConfig._errorPage[code] = path;
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
        // std::cout << line << std::endl;
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

            // std::cout << "i am in the methode" << line << std::endl;
            if (key == "allow_methods")
            {
                // std::cout << "i am in the methode" << std::endl;
                std::istringstream iss(value);
                std::string method;
                while (iss >> method)
                {
                    // here not the same
                    // std::cout << "methode to see: " << method << std::endl;
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
                confLocation.autoindex = (value == "on");
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
            else if (key == "client_size")
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
}
