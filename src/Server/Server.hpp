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
	
	public:
	    Server(const HTTPConfigs& config);
	    void run();
	    void handleNewConnection(int server_fd);
	    void handleClient(int client_fd);
		void closeClient(int client_fd);
	    
	    HTTPConfigs getConfigs();
	    Socket getSocketObject();
	    std::vector<pollfd> getPollfds() { return pollfds; }
	    std::vector<int> getClientSockets() { return _clientSockets; }

};

#endif
