#ifndef KNOTS_HTTPSERVER_HPP
#define KNOTS_HTTPSERVER_HPP

#include <thread>

#include "Router.hpp"
#include "Socket.hpp"
#include "HttpMessage.hpp"

class HttpServer {
private:
    std::atomic<bool> m_isRunning;

    sockaddr_in m_address;
    int m_addrlen;
    Socket m_serverSocket;
    int m_serverPort;

    Router m_router;

    void HandleConnection(Socket clientSocket);
    bool HandleRequest(std::stringstream& ss, const Socket& clientSocketFD);

    void HandleError(const int statusCode, const HttpRequest& req, const Socket& clientSocket) const;
    
    bool SetClientSocketOptions(const Socket& clientSocket) const;
    
public:
    explicit HttpServer();

    void AcceptConnections();
};

#endif