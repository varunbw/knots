#pragma once

#include <unistd.h>

class Socket {
private:
    int m_fd;

public:
    explicit Socket(const int fd) : m_fd(fd) {};
    ~Socket() {
        if (m_fd >= 0) {
            close(m_fd);
        }
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int Get() const {
        return m_fd;
    }
};