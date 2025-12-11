#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "Utils.hpp"

class Socket {
private:
    int m_fd;

public:
    explicit Socket(const int fd) : m_fd(fd) {};
    ~Socket() {
        if (m_fd >= 0) {
            shutdown(m_fd, SHUT_WR);
        }

        // Deplete the send buffer
        while (true) {
            int outstanding = 0;
            ioctl(m_fd, SIOCOUTQ, &outstanding);

            if (outstanding == 0) {
                break;
            }

            usleep(1000);
        }

        // Wait for remote to close socket
        while (true) {
            std::string buffer(4096, 0);
            int receivedBytes = read(m_fd, &buffer[0], buffer.size());

            if (receivedBytes <= 0) {
                break;
            }
        }

        close(m_fd);
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int Get() const {
        return m_fd;
    }
};