#ifndef HTTPRESPONSEUTILS_HPP
#define HTTPRESPONSEUTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

bool isFile(const std::string &path);
bool isDirectory(const std::string &path);
bool isLargeFile(const std::string &path);
bool pathExtension(const std::string &path);
std::string intToHexa(ssize_t num);

std::string iToString(int integer);
std::string ullToStr(unsigned long long ull);
std::string getMimeType(const std::string &path);
std::string getErrorMesssage(const std::string &code);
std::string errorPage(int code);

#endif