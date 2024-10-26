#include "TestHeader.hpp"

// Declare test functions
// void testServerInitialization();
// void testClientRequestHandling();
// void testParseRequestLine();
// void testHTTPResponse();

int main() {
    testServerInitialization();
    testClientRequestHandling();
    testParseRequestLine();
    testHTTPResponse();

    std::cout << "All tests completed." << std::endl;
    return 0;
}
