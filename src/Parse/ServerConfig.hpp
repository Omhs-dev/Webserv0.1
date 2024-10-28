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
#include <iterator> // for std::istream_iterator
#include <numeric>	// for std::accumulate

#include "enumeration.hpp"
#include "LocationConfig.hpp"

struct ServerConfig
{
	std::string     _root;
	std::string     _serverName;
	std::string     _listen;
	std::string     _port;
	std::string     _index;
	std::string     _redirect;
	std::string     _clientMaxBodySize;
	std::map<int, std::string>  _errorPage;
	std::vector<RequestType>    _requestAllowed;
	std::vector<LocationConfig> _locations;
	bool            _autoindex;
	
	// Functions
	std::string getRoot() const;
	std::string getServerName() const;
	std::string getListen() const;
	std::string getPort() const;
	std::string getIndex() const;
	std::string getRedirect() const;
	std::string getClientMaxBodySize() const;
	std::map<int, std::string> getErrorPage() const;
	std::vector<RequestType> getRequestAllowed() const;
	std::vector<LocationConfig> getLocations() const;
	bool getAutoindex() const;
};

#endif