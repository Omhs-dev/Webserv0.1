#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cstdarg>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <filesystem>

#include "ErrorPage.hpp"

bool directoryExist(const char *path);
bool is_path_within_root(const std::string& root, std::string& path);
std::string buildPage(std::vector<std::string> files, std::string path, std::string root);


#endif