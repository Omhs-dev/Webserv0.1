#include "Parse/ConfigParser.hpp"
#include "Server/Server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Check if the user provided a configuration file
    std::string configFile = "webserv.conf"; // Default config file
    if (argc == 2) {
        configFile = argv[1];
    } else if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [configuration_file]" << std::endl;
        return 1;
    }

    try {
        ConfigParser parser;
		parser.parseConfigFile(argv[1]);
        HTTPConfigs configs = parser.getHTTPConfigs();

        // Loop through all the server configs and start the servers
        int serverCount = 0;
        for (const ServerConfig& config : configs._servers) {
            serverCount++;
            std::cout << "server number " << serverCount << std::endl;
            Server server(config);
            server.run();  // Start the server (this could run in a separate thread if you want)
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
