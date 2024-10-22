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

#include "ParserUtils.hpp"

class ConfigParser {
public:
    ConfigParser();
    ~ConfigParser();
    
    HTTPConfigs getHTTPConfigs();
    void parseConfigFile(const std::string& filename);



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
