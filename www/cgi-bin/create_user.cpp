#include <sstream>
#include <iostream>
#include <cstdlib>  // for std::getenvman 
#include <string>
#include <fstream>
#include <filesystem>

void buildResponse(int code, std::string message)
{
    std::ostringstream  resp_body_stream;
    std::ostringstream  headers_stream;

    if (code == 200)
    {
        std::ifstream success_page("www/pages/upload_success.html");
        if (success_page.is_open())
        {
            resp_body_stream << success_page.rdbuf();
            success_page.close();
        }
        else
            resp_body_stream << "<html><body><h1>505</h1><p>Internal server error</p></body></html>";
    }
    else if (code == 405)
    {
        std::ifstream error_page("www/pages/405.html");
        if (error_page.is_open())
        {
            resp_body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            resp_body_stream << "<html><body><h1>405</h1><p>Method not allowed</p></body></html>";
    }
    else if (code == 500)
    {
        std::ifstream error_page("www/pages/500.html");
        if (error_page.is_open())
        {
            resp_body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            resp_body_stream << "<html><body><h1>505</h1><p>Internal server error</p></body></html>";
    }
    

    headers_stream  << "HTTP/1.1 " << std::to_string(code) << " " << message << "\r\n"
                    << "Content-Length: " << resp_body_stream.str().size() << "\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Connection: close\r\n\r\n";

    std::cout << headers_stream.str() << resp_body_stream.str();
}

int main()
{
    std::ofstream       logs("log.txt");
    std::string         requst_body;
    int                 code = 200;
    std::string         message = "OK";

    char* contentLengthStr = std::getenv("CONTENT_LENGTH");
    if (contentLengthStr == nullptr) {
        logs << "Error: CONTENT_LENGTH environment variable not set" << std::endl;
        code = 405;
        message = "Method not allowed";
        logs.close();
        buildResponse(code, message);
        exit(1);
    }

    // Convert the CONTENT_LENGTH string to an integer
    int contentLength = std::atoi(contentLengthStr);

    if (contentLength <= 0)
    {
        logs << "Error: Invalid CONTENT_LENGTH" << std::endl;
        code = 500;
        message = "Internal server error";
        buildResponse(code, message);
        exit(1);
    }
    else
    {
        // Read the POST data (body) from stdin and create a user data file
        std::string request_body(contentLength, '\0');
        std::cin.read(&request_body[0], contentLength);
        // Check if reading was successful
        if (std::cin.gcount() != contentLength)
        {
            logs << "Error: Could not read the complete POST data" << std::endl;
            code = 500;
            message = "Internal server error";
            buildResponse(code, message);
            exit(1);
        }
        else
        {
            std::size_t pos_name = request_body.find("name=");
            std::string name;
            if (pos_name != std::string::npos)
            {
                pos_name += 5;
                std::size_t pos_end = request_body.find("&", pos_name);
                if (pos_end == std::string::npos)
                    pos_end = request_body.length();

                name = request_body.substr(pos_name, pos_end - pos_name);
            }

            std::size_t pos_surname = request_body.find("surname=");
            std::string surname;
            if (pos_surname != std::string::npos)
            {
                pos_surname += 8;
                std::size_t pos_end = request_body.find("&", pos_surname);
                if (pos_end == std::string::npos)
                    pos_end = request_body.length();

                surname = request_body.substr(pos_surname, pos_end - pos_surname);
            }

            std::size_t pos_email = request_body.find("email=");
            std::string email;
            if (pos_email != std::string::npos)
            {
                pos_email += 6;
                std::size_t pos_end = request_body.find("&", pos_email);
                if (pos_end == std::string::npos)
                    pos_end = request_body.length();

                email = request_body.substr(pos_email, pos_end - pos_email);

                // Replace "%40" with "@"
                std::size_t  pos_at = email.find("%40");
                email.replace(pos_at, 3, "@");
            }
            
            std::size_t pos_phone = request_body.find("phone=");
            std::string phone;
            if (pos_phone != std::string::npos)
            {
                pos_phone += 6;
                std::size_t pos_end = request_body.find("&", pos_phone);
                if (pos_end == std::string::npos)
                    pos_end = request_body.length();

                phone = request_body.substr(pos_phone, pos_end - pos_phone);
            }

            std::ofstream file(name);
            if (!file)
            {
                logs << "Error: Could not create the user file: " << name << std::endl;
                code = 500;
                message = "Internal server error";
                buildResponse(code, message);
                exit(1);
            }
            else
            {
                file << "name: " << name << std::endl;
                file << "surname: " << surname << std::endl;
                file << "email: " << email << std::endl;
                file << "phone: " << phone << std::endl;
                file << std::string::npos;
                file.flush();
                file.close();
            }
        }
    }
    buildResponse(200, "OK"); 
    logs.close();

    return 0;
}