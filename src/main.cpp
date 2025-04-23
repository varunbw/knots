#include <iostream>
#include <fstream>

// Networking
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Std lib
#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>

// Project
#include "../include/Utils.hpp"
#include "../include/HttpServer.hpp"


int main(int argc, char** argv) {

    constexpr int port = 8080;
    Log::Info(std::format(
        "Attempting to start server on port {}",
        port
    ));

    HttpServer server(8080);

    Log::Info("Stopping server...");
    return 0;
}