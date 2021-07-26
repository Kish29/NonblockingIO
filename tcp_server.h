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

struct client_info {
    const char *host{};
    int port{};
};

// size代表server能同时处理多少个连接
class tcp_server {
public:
    explicit tcp_server(uint16_t _port, const char *_host = "0.0.0.0");

    ~tcp_server() {
        close(server_sockfd_);
    }

    void startup();

private:
    // linux内核为我们维护的accept_queue的最大值也是2048
    // https://www.cnblogs.com/qiumingcheng/p/9492962.html
    // linux内核通过listen传入的backlog和/proc/sys/net/core/somaxconn取二者的较小值
    static const uint32_t SERVER_MAX_CONNECTION = 2048;

    uint16_t port_;
    const char *host_;
    int server_sockfd_{-1};
    uint32_t connected_num_{0};
    sockaddr_in server_address_{};
    std::map<int, client_info> client_infos{};
    epoll_worker_pool worker_pool;

private:
    [[noreturn]]void poll_for_connect();

};

#endif //EPOLLLEARN_TCP_SERVER_H
