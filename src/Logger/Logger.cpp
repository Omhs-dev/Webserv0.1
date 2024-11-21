#include "Logger.hpp"

// Private Methods
std::string Logger::GetTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    std::ostringstream timestamp;
    timestamp << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
}

void Logger::Log(const std::string &message, const std::string &color, const std::string &prefix)
{
    std::cout << color << "[" << GetTimestamp() << "] [" << prefix << "] : " << message << RESET << std::endl;
}

// Public Methods
Logger::Logger() {}

void Logger::Separator()
{
    std::cout << BLUE << "========================================" << RESET << std::endl;
}

void Logger::VerticalSeparator()
{
    std::cout << BLUE << "----------------------------------------" << RESET << std::endl;
}

void Logger::Itroduction(std::string sring)
{
    Log(sring, GREEN, "INTRO");
}

void Logger::NormalCout(std::string sring)
{
    Log(sring, BLUE, "INFO");
}

void Logger::Cout(std::string sring)
{
    Log(sring, ORANGE, "DEBUG");
}

void Logger::ErrorCout(std::string string)
{
    Log(string, RED, "ERROR");
}

void Logger::Checker(std::string string)
{
    Log(string, GREEN, "CHECK");
}

void Logger::Specifique(std::string elemt, std::string elemMessage)
{
    Log(elemMessage + ": " + elemt, BLUE, "SPECIFIC");
}

void Logger::SpecifiqueForBool(bool elemt, std::string elemMessage)
{
    std::string value = elemt ? "true" : "false";
    Log(elemMessage + ": " + value, BLUE, "SPECIFIC");
}

void Logger::SpecifiqueForInt(int elemt, std::string elemMessage)
{
    Log(elemMessage + ": " + std::to_string(elemt), BLUE, "SPECIFIC");
}

Logger::~Logger() {}
