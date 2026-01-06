#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <poll.h>
#include <sstream>
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

#include "knots/FileHandler.hpp"
#include "knots/HttpServer.hpp"
#include "knots/NetworkIO.hpp"
#include "knots/ThreadPool.hpp"
#include "knots/Utils.hpp"

/*
    @brief Set up the HTTP server

    This takes no parameters for now, server options set are as follows:
        - m_isRunning = false
        - m_serverSocket = socket(AF_INET, SOCK_STREAM, 0)
        - m_router = "config/routes.yaml"
*/
HttpServer::HttpServer(const HttpServerConfiguration& config, const Router& router) :
    m_isRunning(false),
    m_serverSocket(socket(AF_INET, SOCK_STREAM, 0)),
    m_config(config),
    m_router(router) {

    // Check if the socket was created successfully
    if (m_serverSocket.Get() < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket creation failed"
        ));
    }

    ValidateServerConfiguration();

    Log::Info(std::format(
        "Attempting to start server on port {}",
        m_config.port
    ));

    // Set socket options
    // Allow address reuse
    int opt = 1;
    if (setsockopt(m_serverSocket.Get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEADDR"));
    }

    // Allow port reuse
    if (setsockopt(m_serverSocket.Get(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_REUSEPORT"));
    }

    // Set receive timeout
    struct timeval timeout;      
    timeout.tv_sec = 10;  // 10 seconds timeout
    timeout.tv_usec = 0;
    if (setsockopt(m_serverSocket.Get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_RCVTIMEO"));
    }

    // Set send timeout
    if (setsockopt(m_serverSocket.Get(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_SNDTIMEO"));
    }

    // Set TCP keep-alive
    if (setsockopt(m_serverSocket.Get(), SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(MakeErrorMessage("Failed to set SO_KEEPALIVE"));
    }

    // Initialize address information
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_config.port);

    this->m_serverPort = m_config.port;
    this->m_addrlen = sizeof(m_address);

    // Bind the socket to the port
    if (bind(m_serverSocket.Get(), (struct sockaddr*)& m_address, sizeof(m_address)) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Socket binding failed"
        ));
    };

    // Start listening for connections
    if (listen(m_serverSocket.Get(), m_config.maxConnections) < 0) {
        throw std::runtime_error(MakeErrorMessage(
            "HttpServer(): Could not listen"
        ));
    }

    // Start the thread pool
    m_threadPool.InitializeThreadPool(m_config.maxConnections);

    // Spin up a thread to listen to console input
    m_consoleInputHandlerThread = std::jthread(
        [this] () {
            this->HandleConsoleInput();
        }
    );

    // Mark server as running
    m_isRunning = true;

    // Ready to go
    Log::Info(
        std::format("HttpServer(): Server listening on port {}, max {} connections\n",
        m_config.port, m_config.maxConnections
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
    @brief Check that the configuration values are within acceptable limits
*/
void HttpServer::ValidateServerConfiguration() const {

    // Check if the port number is valid
    if (m_config.port <= 0 || m_config.port > 65536) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid port number: {} | Allowed range: 0 - 65536 (both inclusive)",
            m_config.port
        )));
    }

    if (m_config.maxConnections <= 0) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid max connections: {} | Allowed range: > 0",
            m_config.maxConnections
        )));
    }

    if (m_config.inputPollingIntevalMs < 0) {
        throw std::invalid_argument(MakeErrorMessage(std::format(
            "HttpServer(): Invalid input polling timeout: {} ms | Allowed range: > 0ms",
            m_config.inputPollingIntevalMs
        )));
    }

    return;
}

/*
    @brief Listen for console input
*/
void HttpServer::HandleConsoleInput() {

    std::string buffer;

    std::set<std::string> stopCommands = {
        "q", "quit", "stop", "exit"
    };

    auto consoleInputReady = [] () {
        pollfd pfd {
            .fd = STDIN_FILENO,
            .events = POLLIN,
            .revents{}
        };

        return poll(&pfd, 1, 0) > 0;
    };

    // Check input once every while (defined in config) if it's not a test build
    const std::chrono::milliseconds pollingInterval{m_config.inputPollingIntevalMs};

    while (m_isRunning) {
        if (consoleInputReady()) {
            std::cin >> buffer;

            if (stopCommands.contains(buffer)) {
                Shutdown();
                return;
            }
            else {
                Log::Error(std::format(
                    "'{}' is not a valid command, use 'q', 'quit', 'stop', or 'exit' to stop the server",
                    buffer
                ));
            }
        }

        std::this_thread::sleep_for(pollingInterval);
    }

    return;
}


/*
    @brief Gracefully shutdown the server

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
    shutdown(m_serverSocket.Get(), SHUT_RD);

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
        if (setsockopt(clientSocket.Get(), opt.level, opt.option, opt.value, opt.len) < 0) {
            Log::Error(std::format(
                "SetClientSocketOptions(): Could not set options for socket {}",
                clientSocket.Get()
            ));
            return false;
        }
    }

    return true;
}


/*
    @brief Accept incoming connections

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
            m_serverSocket.Get(),
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

        {
            std::scoped_lock<std::mutex> lock(m_activeClientSocketsMutex);
            m_activeClientSockets.erase(clientSocketFD);
        }
    }

    return;
}


/*
    @brief Handle incoming connections
    @param clientSocketFD The socket file descriptor for the client connection
    @param clientAddress The address of the client
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
            clientSocket.Get()
        ));

        HandleError(500, {}, clientSocket);
        return;
    }

    std::stringstream ss;

    constexpr int bufferSize = 32768;
    std::vector<char> buffer(bufferSize);

    while (m_isRunning) {
        ssize_t bytesRead = read(clientSocket.Get(), buffer.data(), bufferSize);

        if (bytesRead == 0)
            break;

        if (bytesRead < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            else {
                Log::Error(std::format(
                    "HandleConnection(): Error reading from socket {}: {}",
                    clientSocket.Get(),
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

    return;
}


/*
    @brief Wrapper for handling any custom behavior for response codes along with `m_errorRouter`
    @param statusCode Status code of the response
    @param req Request object
    @param clientSocket Socket object corresponding to the client
*/
void HttpServer::HandleError(const int statusCode, const HttpRequest& req, const Socket& clientSocket) const {

    HttpResponse res;
    res.SetStatus(statusCode);

    const HandlerFunction* handler = FetchErrorRoute(statusCode);

    if (handler != nullptr) {
        (*handler)(req, res);
    }

    NetworkIO::Send(clientSocket, res.Serialize(), 0);
    return;
}

void HttpServer::AddErrorRoute(short int responseStatusCode, HandlerFunction handler) {
    m_errorRouter[responseStatusCode] = handler;
    return;
}

const HandlerFunction* HttpServer::FetchErrorRoute(short int responseStatusCode) const {
    auto it = m_errorRouter.find(responseStatusCode);
    if (it == m_errorRouter.end()) {
        return nullptr;
    }

    return &(it->second);
}


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
    HttpRequest req;
    const bool parseResult = req.ParseFrom(ss);

    // HTTP 400 - Bad Request
    if (parseResult == false) {
        HandleError(400, req, clientSocket);
        return false;
    }

    const HandlerFunction* handler = m_router.FetchRoute(req);

    // HTTP 404 - Not Found
    if (handler == nullptr) {
        HandleError(404, req, clientSocket);
        return false;
    }

    HttpResponse res;
    res.SetStatus(200);
    (*handler)(req, res);

    const std::optional<std::string> requestConnectionHeader = req.GetHeader("Connection");
    res.SetHeader("Connection", requestConnectionHeader.value_or("close"));

    const std::string resStr = res.Serialize();
    NetworkIO::Send(clientSocket, resStr, 0);

    // Return true if connection header is "keep-alive", else false
    return requestConnectionHeader.value_or("close") == "keep-alive";
}