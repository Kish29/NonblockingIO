//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//
#include "fcntl.h"
#include "unistd.h"
#include "iostream"
#include "getopt.h"
#include "cstring"
#include "sys/socket.h"
#include "tcp_client.h"
#include "arpa/inet.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        hint:
        printf("usage %s -s [server_address] -p [server_port]", argv[0]);
        exit(EXIT_SUCCESS);
    }
    const char *pattern = "s:p:";
    const char *svr_addr{};
    uint16_t svr_port{};
    int opt{};
    while ((opt = getopt(argc, argv, pattern)) != -1) {
        switch (opt) {
            case 's': {
                svr_addr = optarg;
                break;
            }
            case 'p': {
                svr_port = (uint16_t) atoi(optarg);
                break;
            }
            default:
                goto hint;
        }
    }
    tcp_client client{};
    client.cnnto_server2(svr_port, svr_addr);
}


