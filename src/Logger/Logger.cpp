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