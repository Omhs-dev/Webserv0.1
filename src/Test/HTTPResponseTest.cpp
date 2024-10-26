#include "TestHeader.hpp"

void testHTTPResponse() {
    HTTPResponse response;
    response.setStatus("200", "OK");
    response.setBody("<h1>Hello World</h1>");

    std::string data = response.getData();
    assertTrue("testHTTPResponse - Status", data.find("200 OK") != std::string::npos);
    assertTrue("testHTTPResponse - Body", data.find("<h1>Hello World</h1>") != std::string::npos);
}
