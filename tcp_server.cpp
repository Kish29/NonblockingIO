//
// Created by 蒋澳然 on 2021/7/25.
// email: 875691208@qq.com
// $desc
//

#include "tcp_server.h"
#include "cstring"

#define NORMAL_EPOLLEV (EPOLLIN | EPOLLPRI | EPOLLET | EPOLLHUP | EPOLLERR)

void tcp_server::startup() {
    server_sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHECK_PERROR_ABT(server_sockfd_, "Socket create failed!")
//        check_print_abt(server_sockfd_, , __func__, __LINE__);
    // 重利用地址
    int optval = 1;
    setsockopt(server_sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    // 设置发送缓冲区大小为512k
    optval = 512 * 1024;
    setsockopt(server_sockfd_, SOL_SOCKET, SO_SNDBUF, &optval, sizeof optval);
    server_address_.sin_family = AF_INET;
    // 监听本机的任意网卡，任意地址，外部可访问
    if (!host_ ||
        *host_ == '\0' ||
        strcmp(host_, "0.0.0.0") == 0 ||
        strcmp(host_, "0") == 0 ||
        strcmp(host_, "*") == 0) {
        server_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (strcmp(host_, "localhost") == 0) {   // 监听系统回环地址，只有本机程序能够访问
        server_address_.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } else {    // 其他地址，外部访问可能需要开防火墙
        server_address_.sin_addr.s_addr = inet_addr(host_);
    }
    server_address_.sin_port = htons(port_);
    int res = bind(server_sockfd_, (sockaddr *) &server_address_, sizeof server_address_);
    CHECK_PERROR_ABT(res, "Server bind address failed!")
    res = set_fd_non_block(server_sockfd_);
    CHECK_PERROR_ABT(res, "Server set non_blocking failed!")
    // 设置服务器的最大连接数量
    res = listen(server_sockfd_, SERVER_MAX_CONNECTION);
    CHECK_PERROR_ABT(res, "Server listen failed!")
    worker_pool.startup();
    printf("Started server at %s:%d...\n", host_, port_);
    poll_for_connect();
}

[[noreturn]]void tcp_server::poll_for_connect() {
    // 在外部声明以方便资源重复使用
    sockaddr_in client_address{};
    socklen_t claddr_len = sizeof client_address;
    epoll_wrapper poller_for_connect([&](const epoll_event *event, int size) -> void {
        for (int i = 0; i < size; ++i) {
            // 表明有新的连接到来，并且没有超过服务器同时连接的最大数量
            if ((event[i].events & EPOLLIN) && connected_num_ < SERVER_MAX_CONNECTION) {
                int client_fd = accept(server_sockfd_, (sockaddr *) &client_address, &claddr_len);
                if (client_fd > 0) {
                    // 向worker池注册一个连接事件
                    struct client_info info{};
                    info.eplev.data.fd = client_fd;
                    info.eplev.events = NORMAL_EPOLLEV;
                    info.host = inet_ntoa(client_address.sin_addr);
                    info.port = ntohs(client_address.sin_port);
                    fprintf(stdout, "New connection from %s:%d\n", info.host, info.port);
                    // 提交一个新的连接fd
                    worker_pool.submit(info);
                    connected_num_++;
                } else {
                    fprintf(stderr, "Server accept new connection failed!\n");
                }
            } else if (event[i].events & (EPOLLHUP | EPOLLERR)) {
                fprintf(stderr, "Server socket fd on error!\n");
                // 关闭服务器
                close(server_sockfd_);
                abort();
            }
        }
    });
    uint32_t server_event = EPOLLIN | EPOLLPRI | EPOLLET | EPOLLHUP | EPOLLERR;
    poller_for_connect.add_event(server_sockfd_, server_event);
#ifdef __DEBUG__
    char buf[64];
    pthread_getname_np(pthread_self(), buf, 64);
    while (true) {
        printf("%s: Server use epoll to poll new connection...\n", buf);
        poller_for_connect.poll_event(EPOLL_WAIT_TIME);
    }
#else
    while (true) {
        poller_for_connect.poll_event(EPOLL_WAIT_TIME);
    }
#endif
}

