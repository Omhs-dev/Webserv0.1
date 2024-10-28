#include "Utils.hpp"

std::string buildPage(std::vector<std::string> files, std::string path, std::string root){
	std::string page;
	std::string header = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Listing Directory</title><style>@import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');body{padding: 0;margin: 0;box-sizing: border-box;font-family: 'Inter', sans-serif;background-color: #f9f9f9;}.container{--max-width: 1215px;--padding: 1rem;width: min(var(--max-width), 100% - (var(--padding) * 1.2));margin-inline: auto;}a{list-style-type: none;padding: 0;color: black;}.bigLine{width: 100%;height: 1px;background-color: #e0e0e0;margin: 1rem 0;}ul li{list-style-type: '▪️';padding: .2rem 1rem;margin: 0;}a:visited{color: #9e0999;}</style></head>";
	std::string body = "<body><div class=\"container\"><h1>Index of " + path.substr(root.size()) + "</h1><div class=\"bigLine\"></div><ul>";
	
	// ajoute les lien au body
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		body += "<li><a href=\"";
		body += *it;
		body += "\">";
		body += *it;
		body += "</a></li>";
	}
	body += "</ul><div class=\"bigLine\"></div></div></body></html>";
	
	return header + body;
}

bool is_path_within_root(const std::string& root, std::string& path) 
{
		size_t i = 0;
		while (i < root.size() && i < path.size())
		{
			if (root[i] != path[i])
				break;
			i++;
		}
		return (i == root.size());
}

bool directoryExist(const char *path)
{
	struct stat info;
	if (stat(path, &info) != 0)
	{
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

std::string listDirectory(std::string path, std::string root) {
    if (path[0] != '.')
        path.insert(0, ".");
    
    // Log Root and Path (could be replaced with another logging system)
    std::cout << "Root: " << root << "\nPath: " << path << std::endl;

    if (!is_path_within_root(root, path)) {
        std::cerr << "Path is not within root!" << std::endl;
        return ErrorPage::getPage(403);
    }
    std::vector<std::string> files;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        std::cerr << "Failed to open directory: " << path << std::endl;
        return ErrorPage::getPage(404);
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {

        files.push_back(ent->d_name);
    }
    closedir(dir);
    // Use the buildPage function
    std::string body = buildPage(files, path, root);
    std::string header = "HTTP/1.1 200 OK\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    header += "\r\n";

    return header + body;
}


std::string getErrorMessage(int code)
{
	switch (code)
	{
		// 2xx Success
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 204:
			return "No Content";
		
		// 3xx Redirection
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		
		// 4xx Client Errors
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Not Allowed";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 413:
			return "Payload Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 429:
			return "Too Many Requests";

		// 5xx Server Errors
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		
		default:
			return "Unknown Error";
	}
}

/**
 * @brief Fonction pour obtenir le type MIME basé sur l'extension de fichier
 */
std::string getMimeType(const std::string &path)
{
	std::map<std::string, std::string> mimeTypes;

	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".js"] = "application/javascript";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".svg"] = "image/svg+xml";
	mimeTypes[".json"] = "application/json";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".pdf"] = "application/pdf";
	mimeTypes[".zip"] = "application/zip";
	mimeTypes[".tar"] = "application/x-tar";
	mimeTypes[".gz"] = "application/gzip";
	mimeTypes[".mp3"] = "audio/mpeg";
	mimeTypes[".mp4"] = "video/mp4";
	mimeTypes[".avi"] = "video/x-msvideo";
	mimeTypes[".mpeg"] = "video/mpeg";
	mimeTypes[".webm"] = "video/webm";
	mimeTypes[".ogg"] = "video/ogg";
	mimeTypes[".ico"] = "image/x-icon";
	mimeTypes[".webp"] = "image/webp";
	mimeTypes[".woff"] = "font/woff";
	mimeTypes[".woff2"] = "font/woff2";
	mimeTypes[".ttf"] = "font/ttf";
	mimeTypes[".otf"] = "font/otf";
	mimeTypes[".eot"] = "application/vnd.ms-fontobject";
	mimeTypes[".csv"] = "text/csv";
	mimeTypes[".xml"] = "application/xml";
	mimeTypes[".xhtml"] = "application/xhtml+xml";
	mimeTypes[".webmanifest"] = "application/manifest+json";
	mimeTypes[".ico"] = "image/x-icon";
	mimeTypes[".mkv"] = "video/x-matroska";

	std::string::size_type idx = path.rfind('.');
	if (idx != std::string::npos)
	{
		std::string ext = path.substr(idx);
		if (mimeTypes.find(ext) != mimeTypes.end())
		{
			return mimeTypes[ext];
		}
	}
	return "application/octet-stream";
}

// int main() {
//     std::string path = "./test_directory";  // Set the path to your directory here
//     std::string root = "./";  // Root directory

//     std::string response = listDirectory(path, root);

//     std::cout << response << std::endl;

//     return 0;
// }