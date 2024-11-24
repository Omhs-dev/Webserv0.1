#include "cgiHandler.hpp"

static cgiResponse build_response(int code, std::string output)
{
    cgiResponse         response;
    std::ostringstream  body_stream;
    std::ostringstream  headers_stream;
    std::string         error_message;

    if (code == 200)
    {
        response.response_data = output;
        response.status_code = 200;
        response.status = "OK";
        return response;
    }
    else if (code == 400)
    {
        std::ifstream error_page("www/pages/400.html");
        if (error_page.is_open())
        {
            body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            body_stream << "<html><body><h1>400</h1><p>Bad request</p></body></html>";
        error_message = "Bad request";
    }
    else if (code == 405)
    {
        std::ifstream error_page("www/pages/405.html");
        if (error_page.is_open())
        {
            body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            body_stream << "<html><body><h1>405</h1><p>Method not allowed</p></body></html>";
        error_message = "Method not allowed";
    }
	else if (code == 413)
    {
        std::ifstream error_page("www/pages/413.html");
        if (error_page.is_open())
        {
            body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            body_stream << "<html><body><h1>405</h1><p>Request Entity Too Large</p></body></html>";
        error_message = "Request Entity Too Large";
    }
    else if (code == 500)
    {
        std::ifstream error_page("www/pages/500.html");
        if (error_page.is_open())
        {
            body_stream << error_page.rdbuf();
            error_page.close();
        }
        else
            body_stream << "<html><body><h1>500</h1><p>Internal server error</p></body></html>";
        error_message = "Internal server error";
    }

    headers_stream  << "HTTP/1.1 " << code << " " << error_message << "\r\n"
                    << "Content-Length: " << body_stream.str().size() << "\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Connection: close\r\n\r\n";

    response.response_data = headers_stream.str() + body_stream.str();
    response.status_code = code;
    response.status = error_message;

    return response;
}

static void sendCgiResponse(const cgiResponse &response, int socket)
{
    std::string  resp_str = response.response_data;
    send(socket, resp_str.c_str(), resp_str.size(), 0);
}

void handleCGIRequest(const HTTPRequest &Request)
{
    cgiResponse response;
    int socket = Request.getClient()->getClientSocket();

	int reqStateCode = Request.getStateCode();
	std::cout << "request state code : " << reqStateCode << std::endl;
	if (reqStateCode == 413)
	{
		std::cerr << "Error: empty request path (handlecgiRequest())\n";
        sendCgiResponse(build_response(413, ""), socket);
        return ;
	}
    //************1************/
    //get the path to CGI script
    std::string path = Request.getPath();
    if (path.empty())
    {
        std::cerr << "Error: empty request path (handlecgiRequest())\n";
        sendCgiResponse(build_response(500, ""), socket);
        return ;
    }
    std::string scriptPath = "./www" + path;
    std::cout << "CgiLog: request path: " << scriptPath << std::endl;
    //************1*************/

    //******************2*********************/
    //Create env variables for script process
    std::vector<std::string> envVars;
    if (Request.getMethod() != "GET" && Request.getMethod() != "POST")
    {
        std::cerr << "Error: method not allowed (handlecgiRequest())\n";
        sendCgiResponse(build_response(405, ""), socket);
        return ;
    }
    envVars.push_back("REQUEST_METHOD=" + Request.getMethod());
 
    if (!Request.getQuery().empty())
        envVars.push_back("QUERY_STRING=" + Request.getQuery());
    
    std::string postBody = Request.getBody();
    if (Request.getMethod() == "POST")
    {
        if (postBody.empty())
        {
            std::cerr << "Error: empty POST body (handlecgiRequest())\n";
            sendCgiResponse(build_response(400, ""), socket);
            return ;
        }
        // std::cout <<  "CgiLog: body: " << postBody;
        envVars.push_back("CONTENT_LENGTH=" + std::to_string(postBody.length()));
        std::string content_type = Request.getHeaders()["Content-Type"];
        // std::cout << "CgiLog: content-type: " << content_type << std::endl;

        std::string::size_type semicolon_pos = content_type.find(";");
        if (semicolon_pos != std::string::npos)
        {
            std::string mediatype = content_type.substr(0, semicolon_pos);
            envVars.push_back("CONTENT_TYPE=" + mediatype);
            std::string::size_type boundary_pos = content_type.find("boundary=");
            if (boundary_pos != std::string::npos)
            {
                std::string boundary = content_type.substr(semicolon_pos + 11);
                envVars.push_back("BOUNDARY=" + boundary);
            }
        }
        else
            envVars.push_back("CONTENT_TYPE=" + content_type);
    }
    //*********************2******************/

    //*********************3******************/
    // Prepare environment variables for execve (as char* array)
    std::vector<char*> envp;
    for (auto &var : envVars) {
        envp.push_back(const_cast<char*>(var.c_str()));
    }
    envp.push_back(nullptr);
    //*********************3******************/

    //*********************4******************/
    int pipefd_out[2];  // Pipe for reading CGI output
    int pipefd_in[2];   // Pipe for sending POST data to CGI stdin (if necessary)

    if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1)
    {
        std::cerr << "Error: Failed to create pipe" << std::endl;
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process: execute CGI script
        close(pipefd_out[0]);  // Close reading end of output pipe
        dup2(pipefd_out[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipefd_out[1]);

        if (Request.getMethod() == "POST") {
            // Redirect stdin to the input pipe
            close(pipefd_in[1]);  // Close the writing end of the input pipe in the child
            dup2(pipefd_in[0], STDIN_FILENO);  // Redirect stdin to the input pipe
            close(pipefd_in[0]);  // Close the reading end after duplicating it
        }

        char *argv[] = { const_cast<char*>(scriptPath.c_str()), nullptr };
        execve(scriptPath.c_str(), argv, envp.data());  // Execute CGI script
        std::cerr << "Error: Failed to execute CGI script" << std::endl;
        exit(1);
    } 
    else if (pid > 0)
    {
        // Parent process: send POST data to CGI script via pipe (if applicable)
        if (Request.getMethod() == "POST") {
            close(pipefd_in[0]);  // Close reading end of the input pipe in the parent
            write(pipefd_in[1], postBody.c_str(), postBody.size());  // Send the POST data to CGI
            close(pipefd_in[1]);  // Close writing end after sending the data
        }

        // Parent process: read CGI output from the pipe
        close(pipefd_out[1]);  // Close writing end of the output pipe

        char buffer[4096];
        ssize_t bytesRead;
        std::string cgiOutput;

        while ((bytesRead = read(pipefd_out[0], buffer, sizeof(buffer))) > 0)
            cgiOutput.append(buffer, bytesRead);

        close(pipefd_out[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);
            std::cout << "CGI script exited with code: " << exitCode << std::endl;
        } else {
            std::cerr << "Error: CGI script did not exit properly" << std::endl;
        }
        //parseOutput(cgiOutput, response);
        sendCgiResponse( build_response(200, cgiOutput), socket);
    }
}