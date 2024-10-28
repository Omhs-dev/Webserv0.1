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

std::string LocationConfig::getRedirect() const
{
	return redirect;
}

std::string LocationConfig::getLocationPath() const
{
	return locationPath;
}