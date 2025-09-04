#pragma once

#include <arpa/inet.h>
#include <mutex>
#include <set>
#include <thread>

#include "Router.hpp"
#include "Socket.hpp"
#include "HttpMessage.hpp"
#include "ThreadPool.hpp"
#include "Utils.hpp"

class HttpServer {
private:
    // Determine whether server is active or not
    std::atomic<bool> m_isRunning;

    // Hold all active client sockets
    std::mutex m_activeClientSocketsMutex;
    std::set<int> m_activeClientSockets;

    // Server information
    sockaddr_in m_address;
    int m_addrlen;
    Socket m_serverSocket;
    int m_serverPort;

    // Router
    Router m_router;

    // Thread Pool
    ThreadPool m_threadPool;
    std::mutex m_threadPoolMutex;

    // Miscellaneous
    void HandleConsoleInput();
    void Shutdown();
    
    // Handle client connection
    bool SetClientSocketOptions(const Socket& clientSocket) const;
    void HandleConnection(Socket clientSocket);
    bool HandleRequest(std::stringstream& ss, const Socket& clientSocketFD);

    // Error handling
    void HandleError(const int statusCode, const HttpRequest& req, const Socket& clientSocket) const;
    
    
public:
    explicit HttpServer(HttpServerConfiguration& config, const Router& router);
    ~HttpServer();

    void AcceptConnections();
};