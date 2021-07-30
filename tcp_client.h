//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_TCP_CLIENT_H
#define EPOLLLEARN_TCP_CLIENT_H

#include "sys/socket.h"
#include "arpa/inet.h"
#include "unistd.h"

class tcp_client {
public:
    tcp_client();

    explicit tcp_client(uint16_t _port, const char *_srv_addr = "localhost");

    ~tcp_client() {
        close(client_sockfd_);
    }

    int cnnto_server() const;

    int cnnto_server2(uint16_t port, const char *svr_addr);

private:
    int client_sockfd_{};
    bool cnn_svr_set_{};
    // 要连接的服务端地址
    sockaddr_in cnn_svr_addr{};

private:
    void parse_serverarg(uint16_t port, const char *svr_addr);
};


#endif //EPOLLLEARN_TCP_CLIENT_H
