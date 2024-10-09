// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <vector>
#include <sys/stat.h>
#include <map>
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "Socket.hpp"
#include "ConfigParser.hpp"
#include "Location.hpp"

class Server {
public:
    Server(int port, const std::vector<Location>& locations);
    ~Server();
    void run();

private:
    void handleNewConnection();
    void handleClient(int client_fd);
    void closeClient(int client_fd);
    Location matchLocation(const std::string &requestPath);
    bool isMethodAllowed(const Location &location, const std::string &method);

    Socket _serverSocket;
    std::vector<pollfd> _pollfds;
    std::map<int, HTTPRequest> _requests;
    std::vector<Location> _locations;
};

#endif
