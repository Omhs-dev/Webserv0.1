#ifndef PARSERUTILS_HPP
#define PARSERUTILS_HPP

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <limits>
#include <map>
#include <algorithm>

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <iterator> // for std::istream_iterator
#include <numeric>  // for std::accumulate

#include "../Server/Server.hpp"

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

struct LocationConfig
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
    bool _autoindex;
    std::map<int, std::string> errorPage;
    std::vector<RequestType> _requestAllowed;
    std::string _redirect;
    std::vector<LocationConfig> _locations;
};

struct HTTPConfigs
{
    std::vector<ServerConfig> _servers;
};


   //---------function----------//
void printRequestTypes(const std::vector<RequestType> &requestTypes);
void printLocationConfig(const LocationConfig &location);
void printServerConfig(const ServerConfig &server);
RequestType strToRequestType(const std::string &str);
void removeWhiteSpace(std::string &str);
std::string trim(const std::string &str);
std::string toUpperCase(const std::string &str);
// void printConfigParse(const HTTPConfigs &config);
void setKeyValue(std::string &line, std::string &key, std::string &value);

#endif