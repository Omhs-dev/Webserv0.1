#include <sstream>
#include <iostream>
#include <cstdlib>  // for std::getenvman 
#include <string>
#include <fstream>
#include <filesystem>

int main() {
    // Get the value of the CONTENT_LENGTH environment variable

    char* contentLengthStr = std::getenv("CONTENT_LENGTH");
    if (contentLengthStr == nullptr) {
        std::cerr << "Error: CONTENT_LENGTH environment variable not set" << std::endl;
        return 1;
    }

    // Convert the CONTENT_LENGTH string to an integer
    int contentLength = std::atoi(contentLengthStr);

    if (contentLength <= 0) {
        std::cerr << "Error: Invalid CONTENT_LENGTH" << std::endl;
        return 1;
    }

    // Read the POST data (body) from stdin
    std::string body(contentLength, '\0');  // Preallocate a string with the exact length
    std::cin.read(&body[0], contentLength);  // Read contentLength bytes from stdin into the body

    // Check if reading was successful
    if (std::cin.gcount() != contentLength) {
        std::cerr << "Error: Could not read the complete POST data" << std::endl;
        return 1;
    }

    // Output the data to verify (in a real CGI script, you would process the body here)
	std::size_t pos_name = body.find("name=");
	std::string name;
    if (pos_name != std::string::npos)
	{
        pos_name += 5;
        std::size_t pos_end = body.find("&", pos_name);
        if (pos_end == std::string::npos)
            pos_end = body.length();

		name = body.substr(pos_name, pos_end - pos_name);
	}

	std::size_t pos_surname = body.find("surname=");
	std::string surname;
    if (pos_surname != std::string::npos)
	{
        pos_surname += 8;
        std::size_t pos_end = body.find("&", pos_surname);
        if (pos_end == std::string::npos)
            pos_end = body.length();

		surname = body.substr(pos_surname, pos_end - pos_surname);
	}

	std::size_t pos_email = body.find("email=");
	std::string email;
    if (pos_email != std::string::npos)
	{
        pos_email += 6;
        std::size_t pos_end = body.find("&", pos_email);
        if (pos_end == std::string::npos)
            pos_end = body.length();

		email = body.substr(pos_email, pos_end - pos_email);

        // Replace "%40" with "@"
        std::size_t  pos_at = email.find("%40");
        email.replace(pos_at, 3, "@");
	}
    
	std::size_t pos_phone = body.find("phone=");
	std::string phone;
    if (pos_phone != std::string::npos)
	{
        pos_phone += 6;
        std::size_t pos_end = body.find("&", pos_phone);
        if (pos_end == std::string::npos)
            pos_end = body.length();

		phone = body.substr(pos_phone, pos_end - pos_phone);
	}
	std::ofstream file(name);
	if (!file)
		std::cerr << "Error: Could not create the file!" << std::endl;

	file << "name: " << name << std::endl;
	file << "surname: " << surname << std::endl;
	file << "email: " << email << std::endl;
	file << "phone: " << phone << std::endl;
    file << std::string::npos;
    file.flush();
	file.close();

    int status_code = 200;
    std::string status = "OK";
    std::ostringstream resp_body_stream;
    resp_body_stream << "<html><body>"
              <<  "<h1>Hello, " << name << " " << surname << "!</h1>"
              << "</body></html>";
    std::string resp_body = resp_body_stream.str();

    std::ostringstream headers_stream;
    headers_stream << "HTTP/1.1 " << status_code << " OK\r\n"
            << "Content-Length: " << resp_body.size() << "\r\n"
            << "Content-Type: text/html\r\n"
            << "Connection: close\r\n\r\n";

    std::string headers = headers_stream.str();
    std::cout << headers << resp_body;

    return 0;
}