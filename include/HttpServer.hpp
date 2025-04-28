#ifndef KNOTS_HTTPSERVER_HPP
#define KNOTS_HTTPSERVER_HPP

#include "Router.hpp"
#include "Socket.hpp"

class HttpServer {
private:
    sockaddr_in address;
    int addrlen;
    Socket serverSocket;
    int serverPort;

    Router router;

public:
    explicit HttpServer();

    void AcceptConnections();
    void HandleConnection(const int);
};

#endif