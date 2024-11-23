#include "Parse/ConfigParser.hpp"
#include "Server/Server.hpp"
#include "Logger/Logger.hpp"
#include <iostream>

// set max body size
// set cgi in the config
// locat cgi in server locations
// fix hanging connection
// fix leaks
// check the rest of things to do
// continue hunting bugs
// check chunk response again 
// review chunk request again 
// check all appropriate status code for every request
// Upload some file to the server and get it back.
// Setup a list of methods accepted for a certain route (e.g., try to delete something with and without permission).

int main(int argc, char *argv[])
{
	std::string configFile = "config/test.conf";

	if (argc == 2)
		configFile = argv[1];
	else if (argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << " [configuration_file]" << std::endl;
		return 1;
	}

	try
	{
		ConfigParser parser;
		parser.parseConfigFile(configFile);
		std::vector<ServerConfig> configs = parser.getConfigs();

		for (auto &config : configs) {
			parser.printServerConfig(config, false);
		}

		Server server(configs);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
