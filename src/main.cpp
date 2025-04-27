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
#include "Utils.hpp"
#include "HttpServer.hpp"
#include "HttpMessage.hpp"


int main(int argc, char** argv) {

    HttpServer server;
    server.AcceptConnections();

    Log::Info("Stopping server...");
    return 0;
}