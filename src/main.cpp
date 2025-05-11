// Project
#include "Utils.hpp"
#include "HttpServer.hpp"

int main(void) {

    HttpServerConfiguration config = ParseConfigurationFile("config/config.yaml");

    HttpServer server(config);
    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}