// #ifndef SERVER_HPP
// #define SERVER_HPP

// #include <poll.h>
// #include <vector>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <map>
// #include "../Request/HTTPRequest.hpp"
// #include "../Response/HTTPResponse.hpp"
// #include "../Socket/Socket.hpp"
// #include "../Parse/ConfigUtils.hpp"
// #include "../Parse/ConfigParser.hpp"

// class Server {
// public:
//     Server(int port, const std::vector<Location>& locations);
//     ~Server();
//     void run();

// private:
//     void handleNewConnection();
//     void handleClient(int client_fd);
//     void closeClient(int client_fd);
//     Location matchLocation(const std::string &requestPath);
//     bool isMethodAllowed(const Location &location, const std::string &method);

//     Socket _serverSocket;
//     std::vector<pollfd> _pollfds;
//     std::map<int, HTTPRequest> _requests;
//     std::vector<Location> _locations;
// };

// #endif

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <poll.h>
#include "../Parse/ConfigParser.hpp"
#include "../Socket/Socket.hpp"

class Server {
	private:
	    std::vector<int> _serverSockets;
	    std::vector<int> _clientSockets;
	    Socket _socketObject;
	    HTTPConfigs _httpConfigs;
	    ServerConfig serverConfigs;
	    std::vector<pollfd> pollfds;
	
	    void handleNewConnection(int server_fd);
	    void handleClient(int client_fd);
		void closeClient(int client_fd);
	public:
	    Server(const HTTPConfigs& config);
	    void run();

};

#endif
