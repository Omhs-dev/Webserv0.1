#include "TestHeader.hpp"

void assertEqual(const std::string& testName, int actual, int expected) {
    if (actual == expected) {
        std::cout << testName << " passed.\n";
    } else {
        std::cout << testName << " failed: expected " << expected << ", got " << actual << "\n";
    }
}

void assertEqualString(const std::string& testName, const std::string& actual, const std::string& expected) {
	if (actual == expected) {
		std::cout << testName << " passed.\n";
	} else {
		std::cout << testName << " failed: expected " << expected << ", got " << actual << "\n";
	}
}

void assertTrue(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << testName << " passed.\n";
    } else {
        std::cout << testName << " failed.\n";
    }
}
