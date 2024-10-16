#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

// #include <string>
// #include <vector>
// #include <map>

// struct LocationConfig {
//     std::string path;
//     std::string root;
//     std::string index;
//     std::vector<std::string> methods;
//     bool autoindex;
//     LocationConfig() : autoindex(false) {}
// };

// struct ServerConfig {
//     int port;
//     std::string serverName;
//     std::string root;
//     std::string index;
//     std::map<int, std::string> errorPages;
//     std::vector<LocationConfig> locations;
// };

// class ConfigParser {
// public:
//     ConfigParser(const std::string& configFile);
//     ServerConfig getServerConfig() const;

// private:
// 	void trim(std::string &str);
//     ServerConfig serverConfig;
//     void parseConfigFile(const std::string& configFile);
//     void parseServerBlock(std::ifstream& file);
//     void parseLocationBlock(std::ifstream& file, LocationConfig& locationConfig);
//     std::pair<std::string, std::string> parseKeyValue(const std::string& line);
// };

// #endif

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
    bool _autoindex;
    std::map<int, std::string> errorPage;
    std::vector<RequestType> _requestAllowed;
    std::string _redirect;
    std::vector<ConfigLocation> _locations;
};

struct HTTPConfigs
{
    std::vector<ServerConfig> _servers;
};

class ConfigParser {
public:
    ConfigParser();
    ~ConfigParser();
    
    HTTPConfigs getHTTPConfigs();
    void parseConfigFile(const std::string& filename);
    //---------function----------//
	void printRequestTypes(const std::vector<RequestType> &requestTypes);
	void printConfigLocation(const ConfigLocation &location);
	void printServerConfig(const ServerConfig &server);
	RequestType strToRequestType(const std::string &str);
	void removeWhiteSpace(std::string &str);
	std::string trim(const std::string &str);
	std::string toUpperCase(const std::string &str);
	// void printConfigParse(const HTTPConfigs &config);
	void setKeyValue(std::string &line, std::string &key, std::string &value);


private:
    HTTPConfigs httpConfigs;
    std::vector<BlockType> blockStack;
    std::vector<std::string> required;
    std::vector<std::string> defined;

    void parseHttpBlock(std::ifstream& file);
    void parseServerBlock(std::ifstream& file, ServerConfig& serverConfig);
    void parseLocationBlock(std::ifstream& file, ConfigLocation& confLocation);
    // void setKeyValue(const std::string& line, std::string& key, std::string& value);
    // void removeWhiteSpace(std::string& str);
    void locationPath(const std::string& line, ConfigLocation& confLocation);
    void initConfigServer(ServerConfig& serverConf);
};

#endif // CONFIGPARSER_HPP
