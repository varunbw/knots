#include <iostream>
#include <fstream>

// Networking
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

// Std lib
#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>

#include "Utils.hpp"
#include "MessageHandler.hpp"
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
    @brief Set various socket options for the client's socket, check note for more details
    @param clientSocketFD The socket FD for the client

    @return `true` if options could be set successfully, `false` otherwise
*/
bool HttpServer::SetClientSocketOptions(const int clientSocketFD) const {

    // Set receive timeout
    constexpr struct timeval timeout{
        .tv_sec = 10,
        .tv_usec = 0
    };

    /*
        Set TCP keep-alive options
        This makes TCP start probing for inactive connections after 10 seconds
        It sends a probe every 5 seconds, and drops the connection after 3 probes
    */
    constexpr int tcpKeepAlive = 1;
    constexpr int startProbingAfter = 10;
    constexpr int sendProbeInterval = 5;
    constexpr int dropConnAfterProbes = 3;

    const struct SocketOption {
        int level;
        int option;
        void* value;
        socklen_t len;
    } options[] = {
        {SOL_SOCKET,  SO_RCVTIMEO,   (void*)&timeout,             sizeof(timeout)},
        {SOL_SOCKET,  SO_KEEPALIVE,  (void*)&tcpKeepAlive,        sizeof(tcpKeepAlive)},
        {IPPROTO_TCP, TCP_KEEPIDLE,  (void*)&startProbingAfter,   sizeof(startProbingAfter)},
        {IPPROTO_TCP, TCP_KEEPINTVL, (void*)&sendProbeInterval,   sizeof(sendProbeInterval)},
        {IPPROTO_TCP, TCP_KEEPCNT,   (void*)&dropConnAfterProbes, sizeof(dropConnAfterProbes)}
    };

    for (const SocketOption& opt : options) {
        if (setsockopt(clientSocketFD, opt.level, opt.option, opt.value, opt.len) < 0) {
            Log::Error(std::format(
                "SetClientSocketOptions(): Could not set options for socket {}",
                clientSocketFD
            ));
            return false;
        }
    }

    return true;
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
            &clientAddressLen
        );

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

    /*
        If the options could not be set, don't process the request further, as it
        could lead to this thread being permamently occupied by the same client if the client
        does not close the connection
    */
    if (SetClientSocketOptions(clientSocketFD) == false) {
        Log::Error(std::format(
            "Failed to set socket options for socket {}",
            clientSocketFD
        ));

        const std::string errorFileName = "static/server-error.html";

        std::ifstream errorFile(errorFileName, std::ios::binary | std::ios::ate);
        if (errorFile.is_open() == false) {
            Log::Error(std::format(
                "HandleConnection(): Could not open {}, file to send for HTTP 500",
                errorFileName
            ));
            return;
        }
        
        const auto fileSize = errorFile.tellg();
        errorFile.seekg(0);

        std::string responseBody(fileSize, 0);
        errorFile.read(responseBody.data(), fileSize);

        HttpResponse res = MessageHandler::BuildHttpResponse(500, responseBody);
        std::string resStr = MessageHandler::SerializeHttpResponse(res);

        if (send(clientSocketFD, resStr.data(), resStr.size(), 0) < 0) {
            Log::Error(std::format(
                "HandleConnection(): Error sending response to socket {}: {}",
                clientSocketFD,
                strerror(errno)
            ));
        }
        
        return;
    }

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

        /*
            HandleRequest() returns whether or not to keep a connection alive
            If false, break the loop here and stop this connection
        */
        if (HandleRequest(ss, clientSocketFD) == false) {
            break;
        }
    }

    Log::Warning(std::format(
        "HandleConnection(): Connection closed by client {}",
        clientSocketFD
    ));

    return;
}


/*
    @brief Handle an invalid request by sending a HTTP 400 code, along with some body
    @param requestUrl The invalid request url
    @param clientSocketFD The socketFD for the client

    @return void
*/
void HttpServer::HandleInvalidRequest(const std::string& requestUrl, const int clientSocketFD) {

    Log::Error(std::format(
        "HandleRequest(): Invalid request for URL {}",
        requestUrl
    ));

    // ! ToDo: See where this can be improved
    const std::string invalidRequestFilePath = "static/invalid-request.html";
    std::ifstream fileToSend(invalidRequestFilePath, std::ios::binary | std::ios::ate);

    if (fileToSend.is_open() == false) {
        Log::Error(std::format(
            "HandleInvalidRequest(): Could not open response file {}",
            invalidRequestFilePath
        ));
        return;
    }

    const auto fileSize = fileToSend.tellg();
    fileToSend.seekg(0);

    std::string responseBody(fileSize, 0);
    fileToSend.read(responseBody.data(), fileSize);

    HttpResponse res = MessageHandler::BuildHttpResponse(400, std::move(responseBody));
    std::string resStr = MessageHandler::SerializeHttpResponse(res);

    if (send(clientSocketFD, resStr.data(), resStr.size(), 0) < 0) {
        Log::Error(std::format(
            "HandleRequest(): Error sending response to socket {}: {}",
            clientSocketFD,
            strerror(errno)
        ));
    }

    return;
}

/*
    @brief Processes one HTTP request and sends the appropriate response
    @param ss The stringstream containing the raw request
    @param clientSocketFD The socketFD for the client

    @return `true` if connection is to be kept alive, `false` if not
*/
bool HttpServer::HandleRequest(std::stringstream& ss, const int clientSocketFD) {

    // Determine the route
    // ToDo Check if request is valid
    HttpRequest req = MessageHandler::ParseHttpRequest(ss);
    Route route = m_router.GetRoute(req.requestUrl);

    // If route is not valid, return early and send code 400
    if (route.IsValid() == false) {
        HandleInvalidRequest(req.requestUrl, clientSocketFD);
        return true;
    }

    // Valid route found, send the response
    std::ifstream fileToSend(route.filePath, std::ios::binary | std::ios::ate);
    if (fileToSend.is_open() == false) {
        Log::Error(std::format(
            "HandleConnection(): Could not open response file {}",
            route.filePath
        ));
        return true;
    }

    const auto fileSize = fileToSend.tellg();
    fileToSend.seekg(0);

    std::string responseBody(fileSize, 0);
    fileToSend.read(responseBody.data(), fileSize);

    HttpResponse res = MessageHandler::BuildHttpResponse(200, std::move(responseBody));

    /*
        Determine whether to keep the connection alive or not
        This long, convoluted way of checking and setting the Connection header is better than
            writing `res.headers["Connection"] = req.headers["Connection"]`,
        because the above one will create the key "Connection" in `req`, which is not ideal

        Below method will not create the "Connection" header if it is not present in the
        hashmap, saving memory
    */
    bool keepConnectionAlive = false;
    auto it = req.headers.find("Connection");
    if (it != req.headers.end()) {
        keepConnectionAlive = (it->second == "keep-alive");
    }

    if (keepConnectionAlive) {
        res.headers["Connection"] = "keep-alive";
    }
    else {
        res.headers["Connection"] = "close";
    }

    std::string resStr = MessageHandler::SerializeHttpResponse(res);

    if (send(clientSocketFD, resStr.data(), resStr.size(), 0) < 0) {
        Log::Error(std::format(
            "HandleConnection(): Error sending response to socket {}: {}",
            clientSocketFD,
            strerror(errno)
        ));
        return false;
    }

    return keepConnectionAlive;
}