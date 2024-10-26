#include "TestHeader.hpp"

void testServerInitialization() {
    HTTPConfigs config;
    Server server(config);
    
    assertTrue("testServerInitialization: Server pollfds is empty", server.getPollfds().empty());
}
