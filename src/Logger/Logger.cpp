#include "Logger.hpp"

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Itroduction(std::string string)
{
	std::string intro = "Hey i am in :" + string;
	
	std::cout << intro << std::endl;
}

void Logger::Separator()
{
	std::cout << "--------------------------------------" << std::endl;
}

void Logger::VerticalSeparator()
{
	std::cout << "|\n|\n" << std::endl;
}

void Logger::Checker(std::string string)
{
	std::string checker = "Checking again: " + string;
	
	std::cout << checker << std::endl;
}

void Logger::Specifique(std::string elemt, std::string elemMessage)
{
	std::string specifique = elemMessage + " : " + elemt;
	
	std::cout << specifique << std::endl;
}

void Logger::SpecifiqueForBool(bool elemt, std::string elemMessage)
{
	std::string specifique = elemMessage + " : " + (elemt ? "true" : "false");
	
	std::cout << specifique << std::endl;
}

void Logger::SpecifiqueForInt(int elemt, std::string elemMessage)
{
	std::string specifique = elemMessage + " : " + std::to_string(elemt);
	
	std::cout << specifique << std::endl;
}

void Logger::Cout(std::string string)
{
	std::cout << "What is this ▶️" <<  string << std::endl;
}

void Logger::NormalCout(std::string string)
{
	std::cout << string << std::endl;
}

// void Logger::EnumState(HTTPRequest::ParseState state)
// {
//     std::string stateName;
//     switch (state) {
//         case HTTPRequest::START: stateName = "START"; break;
//         case HTTPRequest::IS_REQUEST_LINE: stateName = "REQUEST_LINE"; break;
//         case HTTPRequest::IS_METHODE: stateName = "METHOD"; break;
//         case HTTPRequest::IS_PATH: stateName = "PATH"; break;
//         case HTTPRequest::IS_VERSION: stateName = "VERSION"; break;
//         case HTTPRequest::IS_HEADERS: stateName = "HEADERS"; break;
//         case HTTPRequest::IS_HEADERS_END: stateName = "HEADERS_END"; break;
//         case HTTPRequest::IS_BODY_START: stateName = "BODY_START"; break;
//         case HTTPRequest::IS_BODY_NORMAL: stateName = "BODY_NORMAL"; break;
//         case HTTPRequest::IS_BODY_CHUNKED: stateName = "BODY_CHUNKED"; break;
//         case HTTPRequest::IS_BODY_END: stateName = "BODY_END"; break;
//         case HTTPRequest::COMPLETE: stateName = "COMPLETE"; break;
//         default: stateName = "UNKNOWN";
//     }
//     std::cout << "Current Parse State: " << stateName << std::endl;
// }
