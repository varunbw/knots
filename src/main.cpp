// Project
#include "Utils.hpp"
#include "HttpServer.hpp"

int main(void) {

    HttpServer server;
    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}