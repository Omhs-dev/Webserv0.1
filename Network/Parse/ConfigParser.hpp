#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <limits>
#include <map>
#include <algorithm>

#include "ConfigUtils.hpp"

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
