#include "TestHeader.hpp"

void testClientRequestHandling() {
    int testSocket = 5; // Replace with a valid socket descriptor
    Client client(testSocket);

    try {
        client.handleRequest();
        assertTrue("testClientRequestHandling", true);
    } catch (...) {
        assertTrue("testClientRequestHandling", false);
    }
}
