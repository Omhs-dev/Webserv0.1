#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

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

struct LocationConfig
{
	std::string root;
	bool autoindex = false;
	std::string alias;
	std::string index;
	std::map<int, std::string> redirect;
	std::string locationPath;
	std::map<int, std::string> errorPage;
	std::map<std::string, std::string> cgi;
	std::vector<std::string> requestAllowed;

	// Functions
	std::string getRoot() const;
	bool getAutoindex() const;
	std::string getAlias() const;
	std::string getIndex() const;
	std::map<int, std::string> getRedirect() const;
	std::string getLocationPath() const;
	std::map<int, std::string> getErrorPage() const;
	std::map<std::string, std::string> getCgi() const;
	std::vector<std::string> getRequestAllowed() const;
};

#endif