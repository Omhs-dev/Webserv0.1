#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

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

struct ServerConfig
{
	std::string     _root;
	std::string     _serverName;
	std::string     _listen;
	std::string     _port;
	std::string     _index;
	std::map<int, std::string> _redirect;
	unsigned long long     _clientMaxBodySize = 0;
	std::map<int, std::string>  _errorPage;
	std::vector<std::string>    _requestAllowed;
	std::vector<LocationConfig> _locations;
	bool            _autoindex = false;
	
	// Functions
	std::string getRoot() const;
	std::string getServerName() const;
	std::string getListen() const;
	std::string getPort() const;
	std::string getIndex() const;
	std::map<int, std::string> getRedirect() const;
	unsigned long long getClientMaxBodySize() const;
	std::map<int, std::string> getErrorPage() const;
	std::vector<std::string> getRequestAllowed() const;
	std::vector<LocationConfig> getLocations() const;
	bool getAutoindex() const;
};

#endif