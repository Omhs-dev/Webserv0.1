#ifndef HTTPRESPONSEUTILS_HPP
#define HTTPRESPONSEUTILS_HPP

#include <string>
#include <iostream>

std::string getMimeType(const std::string &path);
std::string getErrorMesssage(const std::string &code);

#endif