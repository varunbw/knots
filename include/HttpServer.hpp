#ifndef KNOTS_HTTPSERVER_HPP
#define KNOTS_HTTPSERVER_HPP

#include "Router.hpp"
#include "Socket.hpp"

class HttpServer {
private:
    sockaddr_in m_address;
    int m_addrlen;
    Socket m_serverSocket;
    int m_serverPort;

    Router m_router;

    void HandleConnection(const int clientSocketFD);
    
    bool HandleRequest(std::stringstream& ss, const int clientSocketFD);
    void HandleInvalidRequest(const std::string& requestUrl, const int clientSocketFD);

    bool SetClientSocketOptions(const int clientSocketFD) const;
    
public:
    explicit HttpServer();

    void AcceptConnections();
};

#endif