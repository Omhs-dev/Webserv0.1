// main.cpp
#include "../include/Server.hpp"
#include "../include/ConfigParser.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
    // Check if the config file is passed as an argument
    if (argc < 2) {
        std::cerr << "Error: Configuration file not provided." << std::endl;
        return 1;
    }

    std::string configFile = argv[1];

    // Parse the configuration file
    ConfigParser parser;
    parser.parse(configFile);

    // Get the port value from the config file
    std::string portStr = parser.getConfigValue("listen");

    if (portStr.empty()) {
        std::cerr << "Error: No port defined in configuration file!" << std::endl;
        return 1;
    }

    // Convert the port string to an integer
    int port;
    try {
        port = std::stoi(portStr);  // This will throw an exception if portStr is not a valid integer
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid port number in configuration file: " << portStr << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Port number out of range in configuration file: " << portStr << std::endl;
        return 1;
    }

    // Get the list of locations from the config
    std::vector<Location> locations = parser.getLocations();

    // Start the server
    try {
        Server server(port, locations);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
