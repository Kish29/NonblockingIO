//
// Created by 蒋澳然 on 2021/7/25.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_TCP_SERVER_H
#define EPOLLLEARN_TCP_SERVER_H

#include "epoll_wrapper.h"
#include "epoll_worker_pool.h"
#include "unistd.h"
#include "sys/socket.h"
#include "vector"
#include "arpa/inet.h"
#include "memory"

// size代表server能同时处理多少个连接
class tcp_server {
public:
    explicit tcp_server(uint16_t _port, const char *_host = "0.0.0.0")
            : port_(_port), host_(_host) {}

    ~tcp_server() {
        close(server_sockfd_);
    }

    void startup();

private:
    static const uint32_t EPOLL_WAIT_TIME = 10000;
    uint16_t port_;
    const char *host_{};
    int server_sockfd_{-1};
    sockaddr_in server_address_{};
    epoll_wrapper poller_for_connect{};
    epoll_worker_pool worker_pool{};

private:
    [[noreturn]]void poll_for_connect();

};

#endif //EPOLLLEARN_TCP_SERVER_H
