#ifndef SERVER_CONFIGUTILS_HPP
#define SERVER_CONFIGUTILS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <iterator> // for std::istream_iterator
#include <numeric>  // for std::accumulate

//---------enum----------//
enum BlockType
{
    HTTP,
    SERVER,
    LOCATION
};

enum RequestType
{
    GET,
    POST,
    DELETE
};

//---------structs----------//

struct ConfigLocation
{
    std::string locationPath;
    std::vector<RequestType> requestAllowed;
    std::string root;
    bool autoindex;
    std::string alias;
    std::string index;
    std::map<int, std::string> errorPage;
    std::string clientMaxBodySize;
    std::map<std::string, std::string> cgi;
    std::string redirect;
};

struct ServerConfig
{
    std::string _listen;
    std::string _index;
    std::string _serverName;
    std::string _clientMaxBodySize;
    std::string _port;
    std::string _root;
    std::map<int, std::string> _errorPage;
    std::vector<RequestType> _requestAllowed;
    std::string _redirect;
    std::vector<ConfigLocation> _locations;
};



struct HTTPConfigs
{
    std::vector<ServerConfig> _servers;
};

//---------function----------//
void printRequestTypes(const std::vector<RequestType> &requestTypes);
void printConfigLocation(const ConfigLocation &location);
void printServerConfig(const ServerConfig &server);
RequestType strToRequestType(const std::string &str);
void removeWhiteSpace(std::string &str);
std::string trim(const std::string &str);
std::string toUpperCase(const std::string &str);
RequestType strToRequestType(const std::string &str);
void setKeyValue(std::string &line, std::string &key, std::string &value);

#endif