// Socket.hpp
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class Socket {
public:
    Socket();
    ~Socket();

    void create();
    void bind(int port);
    void listen(int backlog = 10);
    void setNonBlocking();
    int acceptConnection();
    int getSocketFd() const;

private:
    int _sockfd;
    struct sockaddr_in _address;

    void handleError(const std::string &message);
};

#endif
