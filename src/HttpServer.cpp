#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>

// Networking
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

// Multithreading
#include <thread>

#include "FileHandler.hpp"
#include "HttpServer.hpp"
#include "MessageHandler.hpp"
#include "NetworkIO.hpp"
#include "ThreadPool.hpp"
#include "Utils.hpp"

/*
    @brief Set up the HTTP server

    This takes no parameters for now, server options set are as follows:
        - m_isRunning = false
        - m_serverSocket = socket(AF_INET, SOCK_STREAM, 0)
        - m_router = "config/routes.yaml"
*/
HttpServer::HttpServer(HttpServerConfiguration& config, const Router& router) :
    m_isRunning(false),
    m_serverSocket(socket(AF_INET, SOCK_STREAM, 0)),
    m_router(router),
    m_threadPool(config.maxConnections) {

    // Check if the socket was created successfully
    if (m_serverSocket.get() < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket creation failed"
        ));
    }

    // Parse the configuration file (no way you could've understood what this was)
    // HttpServerConfiguration config;
    // ParseConfigurationFile("config/main.conf", config);

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

    // Spin up a thread to listen to console input
    std::jthread consoleInputHandlerThread(
        [this] () {
            this->HandleConsoleInput();
        }
    );
    consoleInputHandlerThread.detach();

    // Mark server as running
    m_isRunning = true;

    // Ready to go
    Log::Info(
        std::format("HttpServer(): Server listening on port {}, max {} connections",
        config.port, config.maxConnections
    ));
    
    return;
}


/*
    @brief Destructor for HttpServer, handles thread pool cleanup
*/
HttpServer::~HttpServer() {
    m_threadPool.Stop();
}


/*
    @brief Listen for console input
    @return void
*/
void HttpServer::HandleConsoleInput() {

    std::string buffer;
    while (std::cin >> buffer) {
        if (buffer == "stop" || buffer == "exit" || buffer == "quit") {
            Shutdown();
            return;
        }
    }

    return;
}


/*
    @brief Gracefully shutdown the server
    @return void

    Calls shutdown() on the server socket, and sets m_isRunning to false
*/
void HttpServer::Shutdown() {

    m_isRunning = false;

    // Shut down all active connections
    {
        std::scoped_lock<std::mutex> lock(m_activeClientSocketsMutex);
        for (int clientSocketFd : m_activeClientSockets)
            shutdown(clientSocketFd, SHUT_RD);
    }
    
    // Shutdown the server socket
    shutdown(m_serverSocket.get(), SHUT_RD);

    return;
}


/*
    @brief Set various socket options for the client's socket, check note for more details
    @param clientSocketFD The socket FD for the client

    @return `true` if options could be set successfully, `false` otherwise
*/
bool HttpServer::SetClientSocketOptions(const Socket& clientSocket) const {

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
        if (setsockopt(clientSocket.get(), opt.level, opt.option, opt.value, opt.len) < 0) {
            Log::Error(std::format(
                "SetClientSocketOptions(): Could not set options for socket {}",
                clientSocket.get()
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

    /*
        Only accept connections as long as m_isRunning is set to true
        For testing purposes as of now, this is changed to false when the server receives 
        a HTTP request with a header "CloseServer: true"
    */
    while (m_isRunning) {
        // Socket information for the client
        sockaddr_in clientAddress{};
        socklen_t clientAddressLen = sizeof(m_address);

        int clientSocketFD = accept(
            m_serverSocket.get(),
            reinterpret_cast<struct sockaddr*>(&clientAddress),
            &clientAddressLen
        );

        if (clientSocketFD < 0) {
            // Timeout
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }

            if (errno == EINVAL) {
                Log::Info("AcceptConnections(): Server socket has been closed");
                break;
            }
            
            // If the above condition is not satisfied, its an error
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

        {
            std::scoped_lock<std::mutex> lock(m_activeClientSocketsMutex);
            m_activeClientSockets.insert(clientSocketFD);
        }

        // Enqueue a job in the thread pool
        {
            std::scoped_lock<std::mutex> lock(m_threadPoolMutex);
            m_threadPool.EnqueueJob(
                [this, clientSocketFD] () {
                    HandleConnection(Socket(clientSocketFD));
                }
            );
        }
    }

    return;
}


/*
    @brief Handle incoming connections
    @param clientSocketFD The socket file descriptor for the client connection
    @param clientAddress The address of the client
    @return void
*/
void HttpServer::HandleConnection(Socket clientSocket) {

    /*
        If the options could not be set, don't process the request further, as it
        could lead to this thread being permamently occupied by the same client if the client
        does not close the connection
    */
    if (SetClientSocketOptions(clientSocket) == false) {
        Log::Error(std::format(
            "Failed to set socket options for socket {}",
            clientSocket.get()
        ));

        HandleError(500, {}, clientSocket);

        return;
    }

    std::stringstream ss;

    constexpr int bufferSize = 32768;
    std::vector<char> buffer(bufferSize);

    while (m_isRunning) {
        ssize_t bytesRead = read(clientSocket.get(), buffer.data(), bufferSize);

        if (bytesRead == 0)
            break;

        if (bytesRead < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            else {
                Log::Error(std::format(
                    "HandleConnection(): Error reading from socket {}: {}",
                    clientSocket.get(),
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
        if (HandleRequest(ss, clientSocket) == false) {
            break;
        }
    }

    m_activeClientSockets.erase(clientSocket.get());

    Log::Warning(std::format(
        "HandleConnection(): Connection closed to client {}",
        clientSocket.get()
    ));

    return;
}


void HttpServer::HandleError(const int statusCode, const HttpRequest& req, const Socket& clientSocket) const {

    HttpResponse res;

    switch (statusCode) {
        // HTTP 400 - Bad Request
        case 400:
            res = MessageHandler::BuildHttpResponse(400);
            
            res.body = std::format(
                "<!DOCTYPE html>\n<html>\n<body>\n"
                "    <h1 align='center'>400 Bad Request</h1>\n"
                "    <p align='center'>The request URL <b>{}</b> is invalid.</p>\n"
                "    <p align='center'>Please check the URL and try again.</p>\n"
                "</body>\n</html>\n",
                req.requestUrl
            );
            break;

        // HTTP 500 - Internal Server Error
        case 500:
            res = MessageHandler::BuildHttpResponse(500);

            res.body = std::string(
                "<!DOCTYPE html>\n<html>\n<body>\n"
                "   <h1 align='center'>500 Internal Server Error</h1>\n"
                "   <p align='center'>The server encountered an error while processing your request.</p>\n"
                "   <p align='center'>Please try again later.</p>\n"
                "</body>\n</html>\n"
            );
            res.headers["Connection"] = "close";
        break;

        // Some other error
        default:
            Log::Error(std::format(
                "Server encounted error {} that's not specifically handled",
                statusCode
            ));
        
            res = MessageHandler::BuildHttpResponse(statusCode);

            res.body = std::format(
                "<!DOCTYPE html>\n<html>\n<body>\n"
                "   <h1 align='center'>HTTP Code {}</h1>\n"
                "   <p align='center'>The server encountered an unexpected error.</p>\n"
                "   <p align='center'>Please try again later.</p>\n"
                "</body>\n</html>\n",
                statusCode
            );
            res.headers["Connection"] = "close";

            break;
    }

    std::string resStr = MessageHandler::SerializeHttpResponse(res);
    NetworkIO::Send(clientSocket.get(), resStr, 0);

    return;
}

// /*
//     @brief Processes one HTTP request and sends the appropriate response
//     @param ss The stringstream containing the raw request
//     @param clientSocketFD The socketFD for the client

//     @return `true` if connection is to be kept alive, `false` if not
// */
// bool HttpServer::HandleRequest(std::stringstream& ss, const Socket& clientSocket) {

//     // Determine the route
//     HttpRequest req = MessageHandler::ParseHttpRequest(ss);
//     Route route = m_router.GetRoute(req.requestUrl);

//     // If route is not valid, send code 400 and return
//     if (route.IsValid() == false) {
//         HandleError(400, {}, clientSocket);
//         return true;
//     }

//     // Valid route found
//     HttpResponse res = FileHandler::MakeHttpResponseFromFile(200, route.filePath);

//     // Determine whether to keep the connection alive or not
//     auto it = req.headers.find("Connection");
//     if (it != req.headers.end()) {
//         res.headers["Connection"] = it->second;
//     }

//     std::string resStr = MessageHandler::SerializeHttpResponse(res);
//     NetworkIO::Send(clientSocket.get(), resStr, 0);

//     /*
//         This function returns whether to keep this connection alive or not
        
//         If `it` points to valid data (ie, "Connection" key could be found),
//         return whether its value is "keep-alive"
        
//         `it` would point to `req.headers.end()` in case "Connection" key could not be found though
//         In this case, intended behaviour is to close the connection, hence returning `false`
//     */
//     return it != req.headers.end() ? (it->second == "keep-alive") : false;
// }

/*
    @brief Processes one HTTP request and sends the appropriate response
    @param ss The stringstream containing the raw request
    @param clientSocketFD The socketFD for the client

    @return `true` if connection is to be kept alive, `false` if not
*/
bool HttpServer::HandleRequest(
    std::stringstream& ss,
    const Socket& clientSocket
) {
    HttpRequest req = MessageHandler::ParseHttpRequest(ss);
    const HandlerFunction* handler = m_router.FetchRoute(
        req.method, req.requestUrl
    );
    
    if (handler == nullptr) {
        HandleError(400, req, clientSocket);
        return false;
    }

    HttpResponse res = MessageHandler::BuildHttpResponse(200);
    (*handler)(req, res);

    auto it = req.headers.find("Connection");
    if (it != req.headers.end()) {
        res.headers["Connection"] = it->second;
    }

    const std::string resStr = MessageHandler::SerializeHttpResponse(res);
    NetworkIO::Send(clientSocket.get(), resStr, 0);


    Log::Info(std::format("Out of function string {}", resStr));
    Log::Info(std::format("Out of function {}", res.body));

    /*
        Return true if connection header is "keep-alive",
        false if not
    */
    return it != req.headers.end() ?
        (it->second == "keep-alive"):
        false;
    // return false;
}