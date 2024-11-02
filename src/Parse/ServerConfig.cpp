#include "ServerConfig.hpp"

std::string ServerConfig::getRoot() const
{
	return _root;
}

std::string ServerConfig::getServerName() const
{
	return _serverName;
}

std::string ServerConfig::getListen() const
{
	return _listen;
}

std::string ServerConfig::getPort() const
{
	return _port;
}

std::string ServerConfig::getIndex() const
{
	return _index;
}

std::map<int, std::string> ServerConfig::getRedirect() const
{
	return _redirect;
}

std::string ServerConfig::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

std::map<int, std::string> ServerConfig::getErrorPage() const
{
	return _errorPage;
}

std::vector<RequestType> ServerConfig::getRequestAllowed() const
{
	return _requestAllowed;
}

std::vector<LocationConfig> ServerConfig::getLocations() const
{
	return _locations;
}

bool ServerConfig::getAutoindex() const
{
	return _autoindex;
}
