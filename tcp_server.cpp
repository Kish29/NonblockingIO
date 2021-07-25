//
// Created by 蒋澳然 on 2021/7/25.
// email: 875691208@qq.com
// $desc
//

#include "tcp_server.h"
#include "cstring"

void tcp_server::startup() {
    server_sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHECK_PERROR_ABT(server_sockfd_, "Socket create failed!")
//        check_print_abt(server_sockfd_, , __func__, __LINE__);
    int enable = 1;
    setsockopt(server_sockfd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof enable);
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
    res = listen(server_sockfd_, 2048);
    CHECK_PERROR_ABT(res, "Server listen failed!")
    worker_pool.startup();
    printf("Started server at %s:%d...\n", host_, port_);
    poll_for_connect();
}

[[noreturn]]void tcp_server::poll_for_connect() {
    // 在外部声明以方便资源重复使用
    sockaddr_in client_address{};
    socklen_t claddr_len = sizeof client_address;
    poller_for_connect.add_event(server_sockfd_, EPOLLIN | EPOLLET, [&](const epoll_event &event) -> void {
        if (event.events & EPOLLIN) {   // 表明有新的连接到来
            int client_fd = accept(server_sockfd_, (sockaddr *) &client_address, &claddr_len);
            if (client_fd > 0) {
                fprintf(stdout, "New connection from %s\n", inet_ntoa(client_address.sin_addr));
                worker_pool.submit(client_fd);
            }
        }
    });
    while (true) {
        printf("Server use epoll to poll new connection...\n");
        poller_for_connect.poll_event(EPOLL_WAIT_TIME);
    }
}