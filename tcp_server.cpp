//
// Created by 蒋澳然 on 2021/7/25.
// email: 875691208@qq.com
// $desc
//

#include "tcp_server.h"
#include "cstring"

char *event_str(uint32_t events) {
    char *str = new char[128];
    char *p = str;
    if (events & EPOLLIN) {
        strcpy(p, "epoll in ");
    }
    p += strlen("epoll in ");
    if (events & EPOLLHUP) {
        strcpy(p, "epoll hup ");
    }
    p += strlen("epoll hup ");
    if (events & EPOLLERR) {
        strcpy(p, "epoll err ");
    }
    p += strlen("epoll err");
    if (events & EPOLLRDHUP) {
        strcpy(p, "epoll hup");
    }
    return str;
}

tcp_server::tcp_server(uint16_t _port, const char *_host)
        : port_(_port), host_(_host), worker_pool([&](const epoll_event *events, int size) -> void {
    for (int i = 0; i < size; ++i) {
        int fd = events[i].data.fd;
        printf("new event from %d ocurred!\n", fd);
        if (client_infos.find(fd) != client_infos.end()) {
            printf("new event from %s:%d and events is %s\n",
                   client_infos[fd].host,
                   client_infos[fd].port,
                   event_str(events[i].events)
            );
        }
        if (events[i].events & EPOLLIN) {

        } else if (events[i].events & (EPOLLHUP | EPOLLERR)) {
            client_infos.erase(events[i].data.fd);
        }
    }
}) {}

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
    int res = bind(server_sockfd_, (sockaddr * ) & server_address_, sizeof server_address_);
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
                int client_fd = accept(server_sockfd_, (sockaddr * ) & client_address, &claddr_len);
                if (client_fd > 0) {
                    // 向worker池注册一个连接事件
                    struct client_info info{};
                    info.host = inet_ntoa(client_address.sin_addr);
                    info.port = ntohs(client_address.sin_port);
                    fprintf(stdout, "New connection from %s:%d\n", info.host, info.port);
                    // 提交一个新的连接fd
                    worker_pool.submit(client_fd, EPOLLIN | EPOLLOUT | EPOLLET);
                    client_infos.insert(std::make_pair(client_fd, info));
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
    uint32_t conn_event = EPOLLIN | EPOLLET | EPOLLHUP | EPOLLERR;
    poller_for_connect.add_event(server_sockfd_, conn_event);
    while (true) {
        printf("Server use epoll to poll new connection...\n");
        poller_for_connect.poll_event(EPOLL_WAIT_TIME);
    }
}

