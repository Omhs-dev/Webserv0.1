#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>

class HTTPRequest;

class Logger
{
	public:
		Logger();
		
		static void Separator();
		static void VerticalSeparator();
		static void Itroduction(std::string sring);
		static void NormalCout(std::string sring);
		static void Cout(std::string);
		static void ErrorCout(std::string string);
		static void Checker(std::string string);
		static void Specifique(std::string elemt, std::string elemMessage);
		static void SpecifiqueForBool(bool elemt, std::string elemMessage);
		static void SpecifiqueForInt(int elemt, std::string elemMessage);
		
		// request loggers
		// static void EnumState(HTTPRequest::ParseState state);
		
		~Logger();
};

#endif