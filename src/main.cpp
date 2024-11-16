#include "Parse/ConfigParser.hpp"
#include "Server/Server.hpp"
#include "Logger/Logger.hpp"
#include <iostream>

// restructure the checkpathlocation in response
// check the alias implementation
// decide what to use as root or find a way to use the server root
// find solution for the pollin fd and server
// check the rest of things to do
// continue hunting bugs
// check chunk response again 
// get a way to return to the home page once on directory listenning

int main(int argc, char *argv[])
{
	std::string configFile = "config/test.conf";
	if (argc == 2)
	{
		configFile = argv[1];
	}
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

		// Logger::NormalCout("Server configuration:");
		// for (auto &config : configs) {
		// 	parser.printServerConfig(config);
		// }

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
