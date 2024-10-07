#include "TestServer.hpp"

TestServer::TestServer(int port)
	: Server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10) //  socket parameters
{
	std::cout << "Server listening on port: " << port << std::endl;
	std::cout << "http://localhost:" << port << std::endl << std::endl;
}

// void TestServer::accepter()
// {
// 	struct sockaddr_in address = get_socket()->get_address();
// 	int addrlen = sizeof(address);
// 	_new_socket = accept(get_socket()->get_socket(), (struct sockaddr *)&address, (socklen_t*)&addrlen);
// 	read(_new_socket, _buffer, 30000);
// }

void TestServer::handler()
{
	std::string request = getRequestBuffer();
	
	// std::cout << "before request: " << std::endl;
	std::cout << "Request: " << request << std::endl;
	
	std::string line_one;
	// std::cout << "line_one: " << line_one << std::endl;
	for (size_t i = 0; i < request.length(); i++)
	{
		// std::cout << "request[i]: " << request[i] << std::endl;
		if (request[i] == '\n')
			break;
		line_one += request[i];
		// cout << "line_one: " << line_one << endl;
	}
	
	std::string no_http_line;
	for (size_t i = line_one.find('/'); i < line_one.length(); i++)
	{
		// std::cout << "line_one[i] 2: " << line_one[i] << std::endl;
		if (line_one[i] == ' ')
			break;
		no_http_line += line_one[i];
	}
	
	// std::cout << "no_http_line: " << no_http_line << std::endl;
	
	std::string file_type;
	
	// std::cout << "before for loop: " << std::endl;
	for (size_t i = no_http_line.find_last_of('.') + 1; i < no_http_line.length(); i++)
	{
		if (no_http_line[i] != '/')
		{
			file_type += no_http_line[i];
		}
		else
			file_type = "html";
	}
	// std::cout << "after for loop: " << std::endl;
	
	if (file_type == "html")
	{
		const char *headers = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nContent-Type: text/html\nConnection: close\n\n";
		
		std::fstream file;
		std::stringstream html;
		
		file.open("/Users/ohamadou/Desktop/webserv_new/www/index.html", std::ios::in);
		
		if (!file)
			std::cout << "File not found" << std::endl;
		else
		{
			for (std::string line; std::getline(file, line);)
			{
				html << line << std::endl;
			}
		}
		
		const std::string temp = html.str();
		const char *html_content = temp.c_str();
		
		responder("GET", "text/html", headers, html_content);
	}
	else if (file_type == "css")
	{
		const char *headers = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nContent-Type: text/css\nConnection: close\n\n";
		
		std::fstream file;
		std::stringstream css;
		
		file.open("/Users/ohamadou/Desktop/webserv_new/www/style.css", std::ios::in);
		
		if (!file)
			std::cout << "File not found" << std::endl;
		else
		{
			for (std::string line; std::getline(file, line);)
			{
				css << line << std::endl;
			}
		}
		
		const std::string temp = css.str();
		const char *css_content = temp.c_str();
		responder("GET", "text/css", headers, css_content);
	}
	else if (file_type == "js")
	{
		const char *headers = "HTTP/1.1 200 OK\nAccess-Control-Allow-Origin: *\nContent-Type: text/javascript\nConnection: close\n\n";
		
		std::fstream file;
		std::stringstream js;
		
		file.open("/Users/ohamadou/Desktop/webserv_new/www/index.js", std::ios::in);
		
		if (!file)
			std::cout << "File not found" << std::endl;
		else
		{
			for (std::string line; std::getline(file, line);)
			{
				js << line << std::endl;
			}
		}
		
		const std::string temp = js.str();
		const char *js_content = temp.c_str();
		responder("GET", "text/javascript", headers, js_content);
	}
	else
	{
		// std::cout << "404 Not Found" << std::endl;
		const char *headers = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nConnection: close\n\n";
		const char *content = "<html><body><h1>404 Not Found</h1></body></html>";
		responder("GET", "text/html", headers, content);
	}
	
}

void TestServer::responder(std::string reqtype, std::string conttype, const char *headrs, const char *cont)
{
	write(getNewSocket(), headrs, strlen(headrs));
	write(getNewSocket(), cont, strlen(cont));
	
	std::cout << reqtype << " + " << conttype << std::endl;
	
	close(getNewSocket());
}

void TestServer::run()
{
	while (1)
	{
		cout << "-----------------Waiting for connection---------------" << endl;
		accepter();
		handler();
		cout << "-----------------Connection closed---------------------" << endl;
	}
}