#include "ErrorPage.hpp"

/**
 * @brief Retrieves the custom error page if it exists
 */
std::string ErrorPage::getErrorPagesCustom(int statusCode, const std::map<int, std::string>& errorPagesCustom) {
	auto it = errorPagesCustom.find(statusCode);
	if (it == errorPagesCustom.end())
		return "";
	std::ifstream file(it->second.c_str());
	if (!file.is_open()) {
		// Handle file open failure
        std::cerr << "Error: Failed to open custom Error Page: " << it->second << std::endl;
		return "";
	}
	std::stringstream content;
	content << file.rdbuf();
	file.close();
	std::string body = content.str();
	std::string page = "HTTP/1.1 " +  std::to_string(statusCode) + " " + getErrorMessage(statusCode) + "\r\n"
					 + "Content-Type: " + getMimeType(it->second) + "\r\n"
					 + "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n"
					 + body;
	return page;
}

/**
 * @brief Generates a default HTML error page if a custom page is unavailable
 */
std::string ErrorPage::getPage(int statusCode, const std::map<int, std::string>& errorPagesCustom) {
	// Try to get custom error page
	std::string customPage = getErrorPagesCustom(statusCode, errorPagesCustom);
	if (!customPage.empty())
		return customPage;
	// Default error page
	std::string message = getErrorMessage(statusCode);
	std::string body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\">"
					   "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
					   "<title>Error " + std::to_string(statusCode) + "</title>"
					   "<style>@import url('https://fonts.googleapis.com/css2?family=Inter&display=swap');"
					   "body{display:flex;justify-content:center;align-items:center;height:100vh;"
					   "font-family:Inter,sans-serif;background-color:#f1f1f1;}</style></head>"
					   "<body><div><h1>Error " +  std::to_string(statusCode) + "</h1>"
					   "<p>" + message + "</p></div></body></html>";

	std::string responseLine = "HTTP/1.1 " +  std::to_string(statusCode) + " " + message + "\r\n";
	std::string headers = "Content-Type: text/html\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	return responseLine + headers + body;
}
