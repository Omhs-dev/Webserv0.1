#include "TestHeader.hpp"

void testParseRequestLine() {
    Client mockClient(1);
    HTTPRequest request(&mockClient);
    std::string requestLine = "GET /index.html HTTP/1.1";

    request.parseRequest(requestLine);

    assertEqualString("testParseRequestLine - Method", request.getMethod(), "GET");
    assertEqualString("testParseRequestLine - Path", request.getPath(), "/index.html");
    assertEqualString("testParseRequestLine - Version", request.getVersion(), "HTTP/1.1");
}
