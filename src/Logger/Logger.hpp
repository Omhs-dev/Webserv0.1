#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <ctime>

const std::string RESET = "\033[0m";
const std::string GREEN = "\033[32m";       
const std::string BLUE = "\033[36m";        
const std::string RED = "\033[31m";         
const std::string ORANGE = "\033[38;5;208m";

class Logger
{
public:
    Logger();

    // Logging methods
    static void Separator();
    static void VerticalSeparator();
    static void Itroduction(std::string sring);
    static void NormalCout(std::string sring);
    static void Cout(std::string sring);
    static void ErrorCout(std::string string);
    static void Checker(std::string string);
	static void ServerInfos(std::string elemt, std::string elemMessage);
    static void Specifique(std::string elemt, std::string elemMessage);
    static void SpecifiqueForBool(bool elemt, std::string elemMessage);
    static void SpecifiqueForInt(int elemt, std::string elemMessage);

    ~Logger();

private:
    static std::string GetTimestamp();
    static void Log(const std::string &message, const std::string &color, const std::string &prefix);
};

#endif
