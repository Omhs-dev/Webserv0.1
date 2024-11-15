#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <poll.h>
#include <atomic>
#include <csignal>
#include <iostream>

#include "../Parse/ConfigParser.hpp"
#include "../Socket/Socket.hpp"
#include "../Logger/Logger.hpp"

// implement signal handling to stop the server

// implement timeout for the server
	// the connection should be closed after a certain amount of time
	// the server should be able to handle multiple connections at the same time

class Server
{
public:
	enum ServerState {SERVER_START, IS_HANDLE_N_CONNECTION, IS_CLENT_CONNECTION, SERVER_STOP};
private:
	std::vector<int> _serverSockets;
	std::vector<int> _clientSockets;
	bool _serverRun;
	// std::atomic<bool> running(true);
	Socket _socketObject;
	std::vector<ServerConfig> _configs;
	std::vector<pollfd> pollfds;
	ServerState _state;

public:
	Server(const std::vector<ServerConfig> &config);
	~Server();
	
	void run();
	void stop();
	void shutdown();
	// void signalHandler(int signum);
	void handleNewConnection(int server_fd);
	void handleClient(int client_fd);
	void closeClient(int client_fd);

	std::vector<ServerConfig> getConfigs();
	Socket getSocketObject();
	std::vector<pollfd> getPollfds() { return pollfds; }
	std::vector<int> getClientSockets() { return _clientSockets; }
};

#endif
