// ConfigParser.cpp
#include "../include/ConfigParser.hpp"
// ConfigParser.cpp
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>


void ConfigParser::parse(const std::string &file) {
    std::ifstream configFile(file);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file: " << file << std::endl;
        return;
    }

    std::string line;
    bool inServerBlock = false;
    Location currentLocation;
    bool inLocationBlock = false;

    while (std::getline(configFile, line)) {
        trim(line);

        if (isCommentOrEmptyLine(line)) {
            continue;
        }

        // Enter server block
        if (line == "server {") {
            inServerBlock = true;
            continue;
        }

        // Exit server block
        if (line == "}") {
            if (inLocationBlock) {
                _locations.push_back(currentLocation);
                inLocationBlock = false;
            }
            inServerBlock = false;
            continue;
        }

        // Parse location block
        if (inServerBlock && line.substr(0, 9) == "location ") {
            inLocationBlock = true;
            currentLocation = Location();  // Reset current location
            currentLocation.path = line.substr(9, line.length() - 10);  // Capture location path (e.g., "/")
            continue;
        }

        // End of location block
        if (inLocationBlock && line == "}") {
            _locations.push_back(currentLocation);  // Store the completed location
            inLocationBlock = false;
            continue;
        }

        // Parse key-value pairs inside the server or location block
        if (inLocationBlock || inServerBlock) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, ' ') && std::getline(iss, value)) {
                trim(key);
                trim(value);

                // Remove any trailing semicolon from the value
                if (!value.empty() && value.back() == ';') {
                    value.pop_back();
                }

                if (inLocationBlock) {
                    // Process location-specific keys
                    if (key == "methods") {
                        std::istringstream methodStream(value);
                        std::string method;
                        while (methodStream >> method) {
                            currentLocation.methods.push_back(method);  // Capture allowed methods
                        }
                    } else if (key == "root") {
                        currentLocation.root = value;  // Set location-specific root directory
                    } else if (key == "directory_listing") {
                        currentLocation.directoryListing = (value == "on");
                    } else if (key == "redirection") {
                        currentLocation.redirection = value;  // Capture redirection URL
                    }
                } else {
                    // Process server-level keys
                    _configValues[key] = value;
                }
                
                std::cout << "Parsed key: " << key << ", value: " << value << std::endl;
            }
        }
    }

    configFile.close();
}


std::string ConfigParser::getConfigValue(const std::string &key) const {
    auto it = _configValues.find(key);
    if (it != _configValues.end()) {
        return it->second;
    }
    return "";
}

std::vector<Location> ConfigParser::getLocations() const {
    return _locations;
}

void ConfigParser::trim(std::string &str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}

bool ConfigParser::isCommentOrEmptyLine(const std::string &line) {
    return line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == std::string::npos;
}
