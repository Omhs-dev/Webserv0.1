#include "Parse/ConfigParser.hpp"
#include "Server/Server.hpp"
#include "Logger/Logger.hpp"
#include <iostream>

// find solution for the pollin fd and server
// check the rest of things to do
// continue hunting bugs
// check chunk response again 
// get a way to return to the home page once on directory listenning
// check if i can connect to the same port with different tabs
// compare allow_methods of the config and the incoming requests
// check all appropriate status code for every request
// Upload some file to the server and get it back.
// Setup a list of methods accepted for a certain route (e.g., try to delete something with and without permission).


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
