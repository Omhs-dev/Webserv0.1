// ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <map>
#include <vector>
#include "Location.hpp"

class ConfigParser {
public:
    void parse(const std::string &file);
    std::string getConfigValue(const std::string &key) const;
    std::vector<Location> getLocations() const;  // Declaration of getLocations()

private:
    std::map<std::string, std::string> _configValues;
    std::vector<Location> _locations;  // Store parsed locations

    void trim(std::string &str);
    bool isCommentOrEmptyLine(const std::string &line);
};

#endif
