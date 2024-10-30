#include "HTTPResponse.hpp"
#include "HTTPResponseUtils.hpp"

// --- Utils ---
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
		return "text/plain";
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
	else if (code == "204")
	{
		return "No Content";
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
	else
	{
		return "Bad Request";
	}
}
