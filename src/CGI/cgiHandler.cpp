#include "../include/HTTPRequest.hpp"
#include "../include/cgiHandler.hpp"

static std::unique_ptr<cgiResponse> parseOutput(std::string output)
{
    std::unique_ptr<cgiResponse> response = std::make_unique<cgiResponse>();
    if (output.empty()) {
        std::cerr << "Error: Empty CGI output" << std::endl;
        return nullptr;
    }
    std::size_t l_headers = output.find("\r\n\r\n") + 4;
    if (l_headers == std::string::npos)
    {
        std::cerr << "Error: Failed to find header-body separator in CGI output" << std::endl;
        return nullptr;
    }
    std::size_t l_body = output.length() - l_headers;
    response->headers = output.substr(0, l_headers);
    response->body = output.substr(l_headers, l_body);

    std::istringstream status_line(response->headers);
    std::string  protocol, code, status;
    status_line >> protocol >> code >> status;
    response->status_code = atoi(code.c_str());
    response->status = status;
    return response;
}
std::unique_ptr<cgiResponse> handleCGIRequest(const HTTPRequest &httpRequest)
{
    std::unique_ptr<cgiResponse> response;
    std::string path = httpRequest.getPath();
    std::string scriptPath = "www" + path;

    // Create environment variables
    std::vector<std::string> envVars;
    envVars.push_back("REQUEST_METHOD=" + httpRequest.getMethod());
    envVars.push_back("SCRIPT_NAME=" + path);
    if (!httpRequest.getQuery().empty())
        envVars.push_back("QUERY_STRING=" + httpRequest.getQuery());

    // Handle POST-specific environment variables
    std::string postBody;
    if (httpRequest.getMethod() == "POST") {
        postBody = httpRequest.getBody();
        envVars.push_back("CONTENT_LENGTH=" + std::to_string(postBody.length()));
        std::string content_type = httpRequest.getContentType();
        std::cout << "<------>\n" << content_type << "\n<------->" << std::endl;
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

    // Prepare environment variables for execve (as char* array)
    std::vector<char*> envp;
    for (auto &var : envVars) {
        envp.push_back(const_cast<char*>(var.c_str()));
    }
    envp.push_back(nullptr);

    int pipefd_out[2];  // Pipe for reading CGI output
    int pipefd_in[2];   // Pipe for sending POST data to CGI stdin (if necessary)

    if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1) {
        std::cerr << "Error: Failed to create pipe" << std::endl;
        return nullptr;
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process: execute CGI script
        close(pipefd_out[0]);  // Close reading end of output pipe
        dup2(pipefd_out[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipefd_out[1]);

        if (httpRequest.getMethod() == "POST") {
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
        if (httpRequest.getMethod() == "POST") {
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
        response = parseOutput(cgiOutput);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            std::cout << "CGI script exited with code: " << exitCode << std::endl;
        } else {
            std::cerr << "Error: CGI script did not exit properly" << std::endl;
        }
        return response;
    }
    return nullptr;
}
