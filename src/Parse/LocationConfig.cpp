#include "LocationConfig.hpp"

std::string LocationConfig::getRoot() const
{
	return root;
}

bool LocationConfig::getAutoindex() const
{
	return autoindex;
}

std::string LocationConfig::getAlias() const
{
	return alias;
}

std::string LocationConfig::getIndex() const
{
	return index;
}

std::map<int, std::string> LocationConfig::getRedirect() const
{
	return redirect;
}

std::string LocationConfig::getLocationPath() const
{
	return locationPath;
}

std::string LocationConfig::getClientMaxBodySize() const
{
	return clientMaxBodySize;
}

std::map<int, std::string> LocationConfig::getErrorPage() const
{
	return errorPage;
}

std::map<std::string, std::string> LocationConfig::getCgi() const
{
	return cgi;
}

std::vector<RequestType> LocationConfig::getRequestAllowed() const
{
	return requestAllowed;
}