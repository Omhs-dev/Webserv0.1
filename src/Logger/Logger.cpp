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
	std::cout << "|\n|\n|\n" << std::endl;
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

void Logger::Cout(std::string string)
{
	std::cout << "What is this ▶️" <<  string << std::endl;
}