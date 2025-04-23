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

#include "../include/Utils.hpp"
#include "../include/HttpServer.hpp"


HttpServer::HttpServer(const int port) {
    // Parse the configuration file (no way you could've understood what this was)
    HttpServerConfiguration config;
    ParseConfigurationFile("src/config/main.conf", config);

    // Initialize address information
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    this->serverPort = port;
    this->addrlen = sizeof(address);

    // Create an IPv4, TCP/IP socket with no additional flags
    this->socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket creation failed"
        ));
    }

    // Bind the socket to the port
    if (bind(socketFD, (struct sockaddr*)& address, sizeof(address)) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket binding failed"
        ));
    };

    // Start listening for connections
    if (listen(socketFD, config.maxConnections) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Could not listen"
        ));
    }

    // Ready to go
    Log::Info(
        std::format("HttpServer(): Server listening on port {}, max {} connections",
        config.port, config.maxConnections
    ));
    return;
}
