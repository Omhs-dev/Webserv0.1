// Location.hpp
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>

struct Location {
    std::string path;
    std::vector<std::string> methods;
    std::string root;
    bool directoryListing;
    std::string redirection;

    Location() : directoryListing(false) {}
};

#endif
