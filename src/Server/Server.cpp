#include "Server.hpp"

static volatile bool serverRunning = true;

void signalHandler(int signum) {
	if (signum == SIGINT)
	{
		std::cout << "\nInterrupt signal (" << signum << ") received : ";
		serverRunning = false;
	}
}

Server::Server(const std::vector<ServerConfig> &config) : _configs(config)
{
	// _state = SERVER_START;
	_serverRun = true;
	Logger::SpecifiqueForInt(_configs.size(), "Number of servers");
	for (auto &serverConfig : _configs)
	{
		_socketObject.create();
		_socketObject.bind(std::stoi(serverConfig._listen));
		_socketObject.listen(30);
		_socketObject.setNonBlocking();

		Logger::ServerInfos(serverConfig._serverName, "Server name");
		Logger::ServerInfos(serverConfig._listen, "Server listening on port");
		Logger::ServerInfos(serverConfig._index, "Server index file");

		// Add the server socket to the pollfd set
		pollfd serverPollFd;
		serverPollFd.fd = _socketObject.getSocketFd();
		serverPollFd.events = POLLIN; // We're interested in reading new connections
		serverPollFd.revents = 0;
		pollfds.push_back(serverPollFd);

		_serverSockets.push_back(_socketObject.getSocketFd());
	}
	// Create and configure the server socket
}

void Server::run()
{
    std::signal(SIGINT, signalHandler);

    while (serverRunning)
    {
        int activity = poll(pollfds.data(), pollfds.size(), SERVER_DEFAULT_WAIT);
        if (activity < 0)
        {
            Logger::ErrorCout("Error in poll");
            break;
        }
		std::vector<pollfd> pollFds = pollfds;
        for (size_t i = 0; i < pollFds.size(); ++i)
        {
            if (pollFds[i].revents & POLLIN)
            {
                if (std::find(_serverSockets.begin(), _serverSockets.end(), pollFds[i].fd) != _serverSockets.end())
                {
                    Logger::NormalCout("Handling new connection");
                    handleNewConnection(pollFds[i].fd);
                }
                else
                {
                    Logger::SpecifiqueForInt(pollFds[i].fd, "Handling client request");
                    handleClient(pollFds[i].fd);
                }
            }
        }
    }
}


// Handle a new connection on the server socket
void Server::handleNewConnection(int server_fd)
{
	sockaddr_in clientAddr{};
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(server_fd, (struct sockaddr *)&clientAddr, &clientAddrLen);

	if (clientSocket < 0)
	{
		std::cerr << "Failed to accept client connection" << std::endl;
		return;
	}
	
	// Set client socket to non-blocking mode
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	// Add the new client socket to the pollfd set
	pollfd clientPollFd;
	clientPollFd.fd = clientSocket;
	clientPollFd.events = POLLIN;
	pollfds.push_back(clientPollFd);

	Logger::SpecifiqueForInt(clientSocket, "New client connected. fd");
}

void Server::handleClient(int client_fd)
{
	Client client(client_fd, (this));
	client.clientConnectionProcess();
}

// After handling the client, remove it from the pollfd set
void Server::closeClient(int client_fd)
{
	close(client_fd);
	pollfds.erase(std::remove_if(pollfds.begin(), pollfds.end(), [client_fd](const pollfd &pfd)
								{ return pfd.fd == client_fd; }),pollfds.end());
	// Logger::NormalCout("cleanup and close sockets");
}

// --- GETTERS ---

std::vector<ServerConfig> Server::getConfigs()
{
	return _configs;
}

Socket Server::getSocketObject()
{
	return _socketObject;
}

// --- DESTRUCTOR ---

void Server::shutdown()
{
	Logger::NormalCout("Shutting down server...");
	// for (auto &clientSocket : _clientSockets)
	// {
		// Logger::NormalCout("Closing client socket...");
	// 	close(clientSocket);
	// }
	
	for (auto &serverSocket : _serverSockets)
	{
		// Logger::NormalCout("Closing server socket...");
		close(serverSocket);
		for (size_t i = 0; i < pollfds.size(); ++i)
		{
			// Logger::NormalCout("Removing server socket from pollfds...");
			if (pollfds[i].fd == serverSocket)
			{
				pollfds.erase(pollfds.begin() + i);
				break;
			}
		}
	}
	
	// _clientSockets.clear();
	_serverSockets.clear();
	// Logger::NormalCout("Server shutdown complete");
}

Server::~Server()
{
	// stop();
	shutdown();
}
