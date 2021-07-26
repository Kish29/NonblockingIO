//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#include "tcp_client.h"
#include "cassert"
#include "utils.h"
#include "cstring"

tcp_client::tcp_client() {
    client_sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    CHECK_PERROR(client_sockfd_, "Create client socket failed!")
}

tcp_client::tcp_client(uint16_t _port, const char *_srv_addr) : tcp_client() {
    parse_serverarg(_port, _srv_addr);
}

int tcp_client::cnnto_server() const {
    if (!cnn_svr_set) {
        printf("Server address and port must be set first!\n");
        return -1;
    }
    int res = connect(client_sockfd_, (sockaddr *) &cnn_svr_addr, sizeof cnn_svr_addr);
    CHECK_PERROR(res, "Client connect server failed!")
    if (res != -1) {
        printf("Connect to server at %s:%d succeed!", inet_ntoa(cnn_svr_addr.sin_addr), ntohs(cnn_svr_addr.sin_port));
    }
    return res;
}

int tcp_client::cnnto_server2(uint16_t port, const char *svr_addr) {
    parse_serverarg(port, svr_addr);
    return this->cnnto_server();
}

void tcp_client::parse_serverarg(uint16_t port, const char *svr_addr) {
    cnn_svr_addr.sin_family = AF_INET;
    cnn_svr_addr.sin_port = htons(port);
    // inet_pton或inet_addr都可以，只不过inet_pton只支持AF_INET
    cnn_svr_addr.sin_addr.s_addr = inet_addr(strcmp(svr_addr, "localhost") == 0 ? "127.0.0.1" : svr_addr);
    /*if (strcmp(_srv_addr, "localhost") == 0) {
        cnn_svr_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } else {
        inet_pton(AF_INET, _srv_addr, &cnn_svr_addr.sin_addr);
    }*/
    cnn_svr_set = true;
}