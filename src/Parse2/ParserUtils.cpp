#include "ParserUtils.hpp"

// ----------------- Utils ----------------- //

void setKeyValue(std::string &line, std::string &key, std::string &value)
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

void removeWhiteSpace(std::string& line)
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

std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}


std::string toUpperCase(const std::string& str)
{
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

RequestType strToRequestType(const std::string& str)
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

void printRequestTypes(const std::vector<RequestType>& requestTypes)
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

void printConfigLocation(const ConfigLocation& location)
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

void printServerConfig(const ServerConfig& server)
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
