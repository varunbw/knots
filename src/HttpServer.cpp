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
#include <string.h>
#include <vector>

#include "Utils.hpp"
#include "HttpParser.hpp"
#include "HttpServer.hpp"


HttpServer::HttpServer(const int port) :
    serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {

    // Check if the socket was created successfully
    if (serverSocket.get() < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket creation failed"
        ));
    }

    // Check if the port number is valid
    if (port <= 0 || port > 65536) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid port number: {}",
            port
        )));
    }
    
    // Parse the configuration file (no way you could've understood what this was)
    HttpServerConfiguration config;
    ParseConfigurationFile("src/config/main.conf", config);

    if (config.maxConnections <= 0) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid max connections: {}",
            config.maxConnections
        )));
    }

    // Set socket options
    // Allow address reuse
    int opt = 1;
    if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEADDR"));
    }

    // Allow port reuse
    if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEPORT"));
    }

    // Set receive timeout
    struct timeval timeout;      
    timeout.tv_sec = 10;  // 10 seconds timeout
    timeout.tv_usec = 0;
    if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_RCVTIMEO"));
    }

    // Set send timeout
    if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_SNDTIMEO"));
    }

    // Set TCP keep-alive
    if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_KEEPALIVE"));
    }

    // Initialize address information
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    this->serverPort = port;
    this->addrlen = sizeof(address);

    // Bind the socket to the port
    if (bind(serverSocket.get(), (struct sockaddr*)& address, sizeof(address)) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket binding failed"
        ));
    };

    // Start listening for connections
    if (listen(serverSocket.get(), config.maxConnections) < 0) {
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


/*
    @brief Accept incoming connections
    @return void

    This function accepts incoming connections and creates a new socket for each connection.
    It will block until a connection is accepted.
*/
void HttpServer::AcceptConnections() {

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressLen = sizeof(address);

        int clientSocketFD = accept(
            serverSocket.get(),
            reinterpret_cast<struct sockaddr*>(&clientAddress),
            &clientAddressLen);

        if (clientSocketFD < 0) {
            Log::Error("AcceptConnection(): Could not accept connection");
        }
        else {
            Log::Info(std::format(
                "AcceptConnection(): Accepted connection from {}:{}, socket FD {}",
                inet_ntoa(clientAddress.sin_addr),
                ntohs(clientAddress.sin_port),
                clientSocketFD
            ));
        }

        HandleConnection(clientSocketFD);
        close(clientSocketFD);
    }

    return;
}


/*
    @brief Handle incoming connections
    @param clientSocketFD The socket file descriptor for the client connection
    @param clientAddress The address of the client
    @return void
*/
void HttpServer::HandleConnection(const int clientSocketFD) {

    std::stringstream ss;

    constexpr int bufferSize = 32768;
    std::vector<char> buffer(bufferSize);

    while (true) {
        ssize_t bytesRead = read(clientSocketFD, buffer.data(), bufferSize);

        if (bytesRead == 0)
            break;

        if (bytesRead < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            else {
                Log::Error(std::format(
                    "HandleConnection(): Error reading from socket {}: {}",
                    clientSocketFD,
                    strerror(errno)
                ));
                break;
            }
        }

        ss.write(buffer.data(), bytesRead);
        buffer.clear();
    }

    std::cout << ss.str() << '\n';
    return;
}