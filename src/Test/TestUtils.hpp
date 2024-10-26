#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include <iostream>

void assertEqual(const std::string& testName, int actual, int expected);
void assertEqualString(const std::string& testName, const std::string& actual, const std::string& expected);
void assertTrue(const std::string& testName, bool condition);

#endif // TESTUTILS_HPP