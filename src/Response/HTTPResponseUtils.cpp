#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"

std::string iToString(int integer)
{
	std::ostringstream oss;
	oss << integer;
	return oss.str();
}

std::string ullToStr(unsigned long long ull){
	std::ostringstream oss;
	oss << ull;
	return (oss.str());
}

// --- Checkers ---

bool isFile(const std::string &path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool isLargeFile(const std::string &path)
{
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) != 0)
	{
        std::cerr << "Failed to stat largefile: " << path << ". Error: " << std::strerror(errno) << std::endl;
		return false;
	}
	return fileStat.st_size >  MAX_FILE_SIZE;
}

bool isDirectory(const std::string &path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	return S_ISDIR(info.st_mode);
}

bool pathExtension(const std::string &path)
{
	return path.find('.') != std::string::npos;
}

std::string intToHexa(ssize_t num) {
    std::stringstream stream;
    stream << std::hex << num;
    return stream.str();
}

// --- Utils ---

#include <string>
#include <sstream>

std::string errorPage(const std::string &path, const std::string &root)
{
	std::string fullPath = root + path;
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		<< " <html lang=\"en\">\n"
		<< " <head>\n"
		<< " <meta charset=\"UTF-8\">\n"
		<< " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		<< " <title>404 - Page Not Found</title>\n"
		<< " <style>\n"
		<< "    * {\n"
		<< "      margin: 0;\n"
		<< "      padding: 0;\n"
		<< "      box-sizing: border-box;\n"
		<< "    }\n"
		<< "    body {\n"
		<< "      font-family: 'Roboto', sans-serif;\n"
		<< "      background-color: #f4f4f9;\n"
		<< "      color: #333;\n"
		<< "      display: flex;\n"
		<< "      justify-content: center;\n"
		<< "      align-items: center;\n"
		<< "      height: 100vh;\n"
		<< "      overflow: hidden;\n"
		<< "    }\n"
		<< "    .error-container {\n"
		<< "      text-align: center;\n"
		<< "      padding: 40px;\n"
		<< "      max-width: 600px;\n"
		<< "      width: 100%;\n"
		<< "      background-color: #fff;\n"
		<< "      box-shadow: 0 10px 20px rgba(0, 0, 0, 0.1);\n"
		<< "      border-radius: 10px;\n"
		<< "    }\n"
		<< "    .error-code {\n"
		<< "      font-size: 8rem;\n"
		<< "      font-weight: 700;\n"
		<< "      color: #4c49ea;\n"
		<< "    }\n"
		<< "    .error-message {\n"
		<< "      font-size: 2rem;\n"
		<< "      margin: 20px 0;\n"
		<< "      color: #333;\n"
		<< "    }\n"
		<< "    .error-description {\n"
		<< "      color: #666;\n"
		<< "      font-size: 1rem;\n"
		<< "      margin-bottom: 30px;\n"
		<< "    }\n"
		<< "    .back-button {\n"
		<< "      display: inline-block;\n"
		<< "      padding: 12px 24px;\n"
		<< "      font-size: 1rem;\n"
		<< "      font-weight: bold;\n"
		<< "      color: #fff;\n"
		<< "      background-color: #4c49ea;\n"
		<< "      border-radius: 8px;\n"
		<< "      text-decoration: none;\n"
		<< "      transition: background-color 0.3s ease;\n"
		<< "    }\n"
		<< "    .back-button:hover {\n"
		<< "      background-color: #3ecd5e;\n"
		<< "    }\n"
		<< "    .error-code {\n"
		<< "      position: relative;\n"
		<< "      animation: float 2s infinite ease-in-out;\n"
		<< "    }\n"
		<< "    @keyframes float {\n"
		<< "      0%, 100% {\n"
		<< "        transform: translateY(0);\n"
		<< "      }\n"
		<< "      50% {\n"
		<< "        transform: translateY(-10px);\n"
		<< "      }\n"
		<< "    }\n"
		<< " </style>\n"
		<< " </head>\n"
		<< " <body>\n"
		<< " <div class=\"error-container\">\n"
		<< "   <div class=\"error-code\">404</div>\n"
		<< "   <div class=\"error-message\">Page Not Found</div>\n"
		<< "   <div class=\"error-description\">Sorry this page does not exist or has been moved.</div>\n"
		<< "   <a href=\"/\" class=\"back-button\">Go Back Home</a>\n"
		<< " </div>\n"
		<< "</body>\n"
		<< "</html>\n";

	return html.str();
}

std::string getMimeType(const std::string &path)
{
	std::string extension = path.substr(path.find_last_of('.') + 1);
	if (extension == "html" || extension == "htm")
		return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "js")
		return "application/javascript";
	else if (extension == "jpg")
		return "image/jpeg";
	else if (extension == "jpeg")
		return "image/jpeg";
	else if (extension == "png")
		return "image/png";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "svg")
		return "image/svg+xml";
	else if (extension == "ico")
		return "image/x-icon";
	else if (extension == "json")
		return "application/json";
	else if (extension == "md")
		return "text/markdown";
	else if (extension == "pdf")
		return "application/pdf";
	else if (extension == "zip")
		return "application/zip";
	else if (extension == "txt")
		return "text/plain";
	else if (extension == "mp3")
		return "audio/mpeg";
	else if (extension == "mp4")
		return "video/mp4";
	else if (extension == "mpeg")
		return "video/mpeg";
	else if (extension == "webm")
		return "video/webm";
	else if (extension == "ogg")
		return "audio/ogg";
	else if (extension == "wav")
		return "audio/wav";
	else if (extension == "webp")
		return "image/webp";
	else if (extension == "xml")
		return "application/xml";
	else if (extension == "xhtml")
		return "application/xhtml+xml";
	else if (extension == "ttf")
		return "font/ttf";
	else if (extension == "otf")
		return "font/otf";
	else if (extension == "woff")
		return "font/woff";
	else if (extension == "woff2")
		return "font/woff2";
	else if (extension == "eot")
		return "application/vnd.ms-fontobject";
	else if (extension == "sfnt")
		return "application/font-sfnt";
	else if (extension == "csv")
		return "text/csv";
	else
		return "text/html";
}

std::string getErrorMesssage(const std::string &code)
{
	if (code == "200")
	{
		return "OK";
	}
	else if (code == "201")
	{
		return "Created";
	}
	else if (code == "202")
	{
		return "Accepted";
	}
	else if (code == "204")
	{
		return "No Content";
	}
	else if (code == "301")
	{
		return "Moved Permanently";
	}
	else if (code == "302")
	{
		return "Found";
	}
	else if (code == "303")
	{
		return "See Other";
	}
	else if (code == "304")
	{
		return "Not Modified";
	}
	else if (code == "307")
	{
		return "Temporary Redirect";
	}
	else if (code == "308")
	{
		return "Permanent Redirect";
	}
	else if (code == "400")
	{
		return "Bad Request";
	}
	else if (code == "401")
	{
		return "Unauthorized";
	}
	else if (code == "403")
	{
		return "Forbidden";
	}
	else if (code == "404")
	{
		return "Not Found";
	}
	else if (code == "405")
	{
		return "Method Not Allowed";
	}
	else if (code == "500")
	{
		return "Internal Server Error";
	}
	else if (code == "501")
	{
		return "Not Implemented";
	}
	else if (code == "502")
	{
		return "Bad Gateway";
	}
	else if (code == "503")
	{
		return "Service Unavailable";
	}
	else if (code == "504")
	{
		return "Gateway Timeout";
	}
	else if (code == "505")
	{
		return "HTTP Version Not Supported";
	}
	else
	{
		return "Bad Request";
	}
}
