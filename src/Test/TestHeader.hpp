#ifndef TESTHEADER_HPP
#define TESTHEADER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

// Include all headers for the modules you're testing
#include "../Server/Server.hpp"
#include "../Server/Client.hpp"
#include "../Request/HTTPRequest.hpp"
#include "../Response/HTTPResponse.hpp"
#include "../Parse/ConfigParser.hpp"
#include "../Parse/LocationConfig.hpp"
#include "../Parse/ServerConfig.hpp"

// Include the test utility functions
#include "TestUtils.hpp"
// Declare test functions
void testServerInitialization();
void testClientRequestHandling();
void testParseRequestLine();
void testHTTPResponse();

#endif // TESTHEADER_HPP
