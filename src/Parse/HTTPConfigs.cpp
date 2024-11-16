#include "HTTPConfigs.hpp"

HTTPConfigs::HTTPConfigs()
{
	// std::cout << "HTTPConfigs default constructor" << std::endl;
}

HTTPConfigs::HTTPConfigs(std::vector<ServerConfig> servers)
{
	this->_servers = servers;
}

HTTPConfigs::HTTPConfigs(const HTTPConfigs &copy)
{
	this->_servers = copy._servers;
	std::cout << "HTTPConfigs copy constructor" << std::endl;
}

HTTPConfigs &HTTPConfigs::operator=(const HTTPConfigs &copy)
{
	if (this == &copy)
		return *this;
	this->_servers = copy._servers;
	return (*this);
}

std::vector<ServerConfig> HTTPConfigs::getServers() const
{
	return this->_servers;
}

HTTPConfigs::~HTTPConfigs()
{
	// std::cout << "HTTPConfigs destructor" << std::endl;
}