//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//
#include "tcp_server.h"
#include "cstdlib"

int main(int argc, char *argv[]) {
    int port{};
    if (argc < 2) {
        port = 5555;
    } else {
        port = atoi(argv[1]);
    }
    tcp_server server(port);
    server.startup();
}