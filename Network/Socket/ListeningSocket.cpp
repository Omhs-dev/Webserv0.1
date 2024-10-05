#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interf, int bklog)
				:BindingSocket(domain, service, protocol, port, interf)
{
	_backlog = bklog;
	start_listening();
	check_connection(_listening);
}

void ListeningSocket::start_listening()
{
	_listening = listen(get_connection(), _backlog);
}