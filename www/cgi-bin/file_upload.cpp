#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::ofstream       logs("log.txt");
    std::ostringstream  resp_body_stream;
    std::ostringstream  headers_stream;
    int                 status_code = 200;
    std::string         status_string = "OK";

    // Read content length
    std::string content_length_str = getenv("CONTENT_LENGTH") ? getenv("CONTENT_LENGTH") : "0";
    std::string boundary_str = getenv("BOUNDARY");
    int content_length = std::stoi(content_length_str);

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
            start += 10; // Move past "filename=\""
            std::string::size_type end = post_data.find("\"", start);
            filename = post_data.substr(start, end - start);
            // Check if the filename ends with .txt
            if (filename.substr(filename.find_last_of(".") + 1) != "txt")
            {
                status_code = 415;
                status_string = "Unsupported Media Type";
                logs << "Error: Unsupported file format: " << filename << std::endl;
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
    if (status_code == 200)
    {
        logs <<  "Creating new file: " << filename << std::endl;
        // Save the file to the server
        std::ofstream ofs("www/uploads/" + fs::path(filename).filename().string(), std::ios::binary);
        if (!ofs)
        {
            status_code = 500;
            status_string = "Internal Server Error";
            logs <<  "Error: Could not save the file";
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

    
    if (status_code == 200)
        resp_body_stream << "<html><body><h1>File uploaded successfully: " << filename << "</h1></body></html>";
    else if (status_code == 415)
        resp_body_stream << "<html><body><h1>Wrong file format: " << filename << "<br>Please upload a file in .txt format</br></h1></body></html>";
    else if (status_code == 500)
        resp_body_stream << "<html><body><h1>Error: Could not save the file</h1></body></html>";

    //Response headers
    headers_stream  << "HTTP/1.1 " << std::to_string(status_code) << " " << status_string << "\r\n"
                    << "Content-Length: " << resp_body_stream.str().size() << "\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Connection: close\r\n\r\n";
    logs << "<<--- Response --->>";
    logs << headers_stream.str();
    logs << resp_body_stream.str();
    std::cout << headers_stream.str() << resp_body_stream.str();
    logs.close();
    return 0;
}
