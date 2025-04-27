#ifndef KNOTS_SOCKET_HPP
#define KNOTS_SOCKET_HPP

class Socket {
private:
    int fd;

public:
    explicit Socket(const int fd) : fd(fd) {};
    ~Socket() {
        if (fd >= 0)
            close(fd);
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int get() const {
        return fd;
    }
};


#endif