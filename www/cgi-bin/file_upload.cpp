#include <iostream>
#include <fstream>
#include <string>
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
            resp_body_stream << "<html><body><h1>200</h1><p>Success</p></body></html>";
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
    else if (code == 415)
    {
        std::ifstream error_page("www/pages/415.html");
        if (error_page.is_open())
        {
            resp_body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            resp_body_stream << "<html><body><h1>405</h1><p>File extension not allowed</p></body></html>";
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

int main() {
    std::ofstream       logs("log.txt");
    std::ostringstream  resp_body_stream;
    std::ostringstream  headers_stream;
    int                 code = 200;
    std::string         message = "OK";

    logs << "Executing script\n";
    // Read content length
    char* contentLengthStr = std::getenv("CONTENT_LENGTH");
    if (contentLengthStr == nullptr) {
        logs << "Error: CONTENT_LENGTH environment variable not set" << std::endl;
        code = 405;
        message = "Method not allowed";
        logs.close();
        buildResponse(code, message);
        exit(1);
    }
    std::string boundary_str = getenv("BOUNDARY");
    int content_length = std::stoi(contentLengthStr);

    // Read POST data from stdin
    std::string post_data;
    post_data.resize(content_length);
    std::cin.read(&post_data[0], content_length);

    // Extract the filename and content type from the POST data
    std::string filename;
    std::string file_content;
    std::string::size_type start = post_data.find(boundary_str);
    if (start != std::string::npos) {
        // Parse the filename and content
        start = post_data.find("filename=\"", start);
        if (start != std::string::npos) {
            logs << "Found file name\n";
            start += 10; // Move past "filename=\""
            std::string::size_type end = post_data.find("\"", start);
            filename = post_data.substr(start, end - start);
            // Check if the filename ends with .txt
            if (filename.substr(filename.find_last_of(".") + 1) != "txt")
            {
                code = 415;
                message = "Unsupported Media Type";
                logs << "Error: Unsupported file format: " << filename << std::endl;
                buildResponse(code, message);
                logs.close();
                exit(1);
            }
            else
            {
                // Extract the file content
                start = post_data.find("\r\n\r\n") + 4; // Move past the header
                std::string::size_type end_of_file = post_data.find( "--" + boundary_str, start); // find position of last boundary
                file_content = post_data.substr(start, end_of_file - start); // extract file content
            }
        }
    }
    if (code == 200)
    {
        logs <<  "Creating new file: " << filename << std::endl;
        // Save the file to the server
        std::ofstream ofs("www/uploads/" + std::filesystem::path(filename).filename().string(), std::ios::binary);
        if (!ofs)
        {
            code = 500;
            message = "Internal Server Error";
            logs <<  "Error: Could not save the file to";
            logs << "../uploads/" + std::filesystem::path(filename).filename().string() << std::endl;
            logs.close();
            buildResponse(code, message);
            exit (1);
        }
        else
        {
            ofs.write(file_content.data(), file_content.size());
            ofs.close();
            logs << "<<<--- START OF FILE CONTENT --->>>\n";
            logs << file_content;
            logs << "<<<--- END OF FILE CONTENT --->>>\n";
            logs << "File uploaded successfully: " << filename << std::endl;
        }
    }

    buildResponse(200, "OK");
    logs.close();
    return 0;
}
