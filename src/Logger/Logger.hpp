#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>

class Logger
{
	public:
		Logger();
		
		static void Itroduction(std::string sring);
		static void Separator();
		static void VerticalSeparator();
		static void Checker(std::string string);
		static void Cout(std::string);
		static void Specifique(std::string elemt, std::string elemMessage);
		
		~Logger();
};


#endif