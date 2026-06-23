#pragma once

#include <arpa/inet.h>
#include <mutex>
#include <netinet/in.h>
#include <set>
#include <thread>

#include "knots/HttpMessage.hpp"
#include "knots/Router.hpp"
#include "knots/Socket.hpp"
#include "knots/ThreadPool.hpp"
#include "knots/utils/Config.hpp"

class HttpServer {
private:
    // Determine whether server is active or not
    std::atomic<bool> m_isRunning;

    // Hold all active client sockets
    std::mutex m_activeClientSocketsMutex;
    std::set<int> m_activeClientSockets;

    std::jthread m_consoleInputHandlerThread;

    // Server information
    sockaddr_in m_address;
    int m_addrlen;
    Socket m_serverSocket;
    int m_serverPort;

    const HttpServerConfiguration m_config;

    // Routers
    Router m_router;
    std::unordered_map<short int, HandlerFunction> m_errorRouter;

    // Thread Pool
    ThreadPool m_threadPool;
    std::mutex m_threadPoolMutex;

    // Miscellaneous
    void SetServerSocketOptions();
    void ValidateServerConfiguration() const;
    void HandleConsoleInput();
    
    // Handle client connection
    bool SetClientSocketOptions(const Socket& clientSocket) const;
    void HandleConnection(Socket clientSocket, const sockaddr_in clientAddress);
    bool HandleRequest(
        std::stringstream& ss,
        const Socket& clientSocketFD,
        const sockaddr_in& clientAddress
    );
    void HandleError(
        const int statusCode,
        const HttpRequest& req,
        const Socket& clientSocket,
        const sockaddr_in& clientAddress
    ) const;
    
public:
    explicit HttpServer(const HttpServerConfiguration config, const Router& router);
    ~HttpServer();
    void Shutdown();

    bool IsReady() const;

    void AcceptConnections();

    void AddErrorRoute(short int responseStatusCode, HandlerFunction handler);
    const HandlerFunction* FetchErrorRoute(short int responseStatusCode) const;
};