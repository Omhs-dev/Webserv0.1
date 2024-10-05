#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interf, int bklog)
				:BindingSocket(domain, service, protocol, port, interf)
{
	_backlog = bklog;
	start_listening(get_socket(), _backlog);
	check_connection(get_connection());
}

void ListeningSocket::start_listening(int sock, int backlog)
{
	_listening = listen(sock, backlog);
}
