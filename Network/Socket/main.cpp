#include "ListeningSocket.hpp"

int main()
{
	std::cout << "starting server" << std::endl;
	std::cout << "Binding server" << std::endl;
	BindingSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
	std::cout << "Listening socket" << std::endl;
	ListeningSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);
	std::cout << "Server started" << std::endl;
	return 0;
}