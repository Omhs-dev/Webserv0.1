#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int clientSocket);
    void handleRequest();
    ~Client();

private:
    int clientSocket;
    void sendResponse(const std::string& response);
};

#endif
