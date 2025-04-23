#ifndef KNOTS_HTTPSERVER_HPP
#define KNOTS_HTTPSERVER_HPP

class HttpServer {
private:
    sockaddr_in address;
    int addrlen;
    int socketFD;
    int serverPort;

public:
    explicit HttpServer(const int port);
    ~HttpServer() {
        close(socketFD);
    };
};

#endif