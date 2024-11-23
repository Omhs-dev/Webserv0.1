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
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <iterator>
#include <numeric>

#include "enumeration.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"



class ConfigParser
{
	private:
		std::vector<ServerConfig> configs;
		std::vector<BlockType> blockStack;
		std::vector<std::string> required;
		std::vector<std::string> defined;
	
		void parseHttpBlock(std::ifstream &file);
		void parseServerBlock(std::ifstream &file, ServerConfig &serverConfig);
		void parseLocationBlock(std::ifstream &file, LocationConfig &confLocation);
		void locationPath(const std::string &line, LocationConfig &confLocation);
		void initConfigServer(ServerConfig &serverConf);
	
	public:
		ConfigParser();
		~ConfigParser();
	
		std::vector<ServerConfig> getConfigs();
		void parseConfigFile(const std::string &filename);
		
		//---------utils function----------//
		void printLocationConfig(const LocationConfig &location);
		void printServerConfig(const ServerConfig& server, bool pswitch = false);
		void removeWhiteSpace(std::string &str);
		std::string trim(const std::string &str);
		std::string toUpperCase(const std::string &str);
		void setKeyValue(std::string &line, std::string &key, std::string &value);
};

// getServerRoot
// getLocationRoot

#endif // CONFIGPARSER_HPP
