#include "header.hpp"
#include "./Network/Parse/ConfigParser.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config-file>" << std::endl;
		return 1;
	}
	try
	{
		ConfigParser parser;
		parser.parseConfigFile(argv[1]);
	
		HTTPConfigs configs = parser.getHTTPConfigs();
		// Print parsed configuration
		std::cout << "HTTP Configuration:" << std::endl;
		for (std::vector<ServerConfig>::const_iterator it = configs._servers.begin(); it != configs._servers.end(); ++it)
		{
			printServerConfig(*it);
		}
		// TCPServer server(configs);
		// std::cout << "Configuration parsing and printing completed successfully." << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}