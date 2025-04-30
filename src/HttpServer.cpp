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

HttpServer::HttpServer() :
    m_serverSocket(socket(AF_INET, SOCK_STREAM, 0)),
    m_router("config/routes.yaml")
    {

    // Check if the socket was created successfully
    if (m_serverSocket.get() < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket creation failed"
        ));
    }

    // Parse the configuration file (no way you could've understood what this was)
    HttpServerConfiguration config;
    ParseConfigurationFile("config/main.conf", config);

    Log::Info(std::format(
        "Attempting to start server on port {}",
        config.port
    ));

    // Check if the port number is valid
    if (config.port <= 0 || config.port > 65536) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid port number: {}",
            config.port
        )));
    }

    if (config.maxConnections <= 0) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid max connections: {}",
            config.maxConnections
        )));
    }

    // Set socket options
    // Allow address reuse
    int opt = 1;
    if (setsockopt(m_serverSocket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEADDR"));
    }

    // Allow port reuse
    if (setsockopt(m_serverSocket.get(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEPORT"));
    }

    // Set receive timeout
    struct timeval timeout;      
    timeout.tv_sec = 10;  // 10 seconds timeout
    timeout.tv_usec = 0;
    if (setsockopt(m_serverSocket.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_RCVTIMEO"));
    }

    // Set send timeout
    if (setsockopt(m_serverSocket.get(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_SNDTIMEO"));
    }

    // Set TCP keep-alive
    if (setsockopt(m_serverSocket.get(), SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_KEEPALIVE"));
    }

    // Initialize address information
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(config.port);

    this->m_serverPort = config.port;
    this->m_addrlen = sizeof(m_address);

    // Bind the socket to the port
    if (bind(m_serverSocket.get(), (struct sockaddr*)& m_address, sizeof(m_address)) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket binding failed"
        ));
    };

    // Start listening for connections
    if (listen(m_serverSocket.get(), config.maxConnections) < 0) {
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
        socklen_t clientAddressLen = sizeof(m_address);

        int clientSocketFD = accept(
            m_serverSocket.get(),
            reinterpret_cast<struct sockaddr*>(&clientAddress),
            &clientAddressLen);

        if (clientSocketFD < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            
            Log::Error(std::format(
                "AcceptConnection(): Could not accept connection"
            ));
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
        break; // ! ToDo remove this
    }

    std::cout << ss.str() << '\n';

    HandleRequest(ss, clientSocketFD);
    return;
}


/*
    @brief Processes one HTTP request and sends the appropriate response
    @param ss The stringstream containing the raw request
    @param clientSocketFD The socketFD for the client

    @return void
*/
void HttpServer::HandleRequest(std::stringstream& ss, const int clientSocketFD) {

    // Determine the route
    HttpRequest req = HttpParser::ParseHttpRequest(ss);
    Route route = m_router.GetRoute(req.requestUrl);

    // If route is not valid, return early and send code 400
    if (route.IsValid() == false) {
        Log::Error(std::format(
            "HandleRequest(): Invalid request for URL {}",
            req.requestUrl
        ));

        // ! ToDo: Change this
        const std::string invalidRequestFilePath = "static/invalid-request.html";
        std::ifstream fileToSend(invalidRequestFilePath, std::ios::binary);

        std::vector<char> response(
            (std::istreambuf_iterator<char>(fileToSend)),
            std::istreambuf_iterator<char>()
        );

        if (send(clientSocketFD, response.data(), response.size(), 0) < 0) {
            Log::Error(std::format(
                "HandleRequest(): Error sending response to socket {}: {}",
                clientSocketFD,
                strerror(errno)
            ));
        }

        return;
    }

    // Valid route found, send the response
    std::ifstream fileToSend(route.filePath, std::ios::binary);
    if (fileToSend.is_open() == false) {
        Log::Error(std::format(
            "HandleConnection(): Could not open response file {}",
            route.filePath
        ));
        return;
    }

    std::vector<char> response(
        (std::istreambuf_iterator<char>(fileToSend)),
        std::istreambuf_iterator<char>()
    );

    if (send(clientSocketFD, response.data(), response.size(), 0) < 0) {
        Log::Error(std::format(
            "HandleConnection(): Error sending response to socket {}: {}",
            clientSocketFD,
            strerror(errno)
        ));
    }

    return;
}