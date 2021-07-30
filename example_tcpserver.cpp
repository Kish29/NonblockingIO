//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//
#include "tcp_server.h"
#include "cstdlib"

int main(int argc, char *argv[]) {
    const char *pattern = "n:p:h";
    uint32_t thread_num = CPU_CORE;
    uint16_t svr_port = 5555;
    int opt{};
    while ((opt = getopt(argc, argv, pattern)) != -1) {
        switch (opt) {
            case 'n': {
                thread_num = atoi(optarg);
                break;
            }
            case 'p': {
                svr_port = (uint16_t) atoi(optarg);
                break;
            }
            case 'h':
            default:
                printf("usage %s -n [thread number] -p [server_port]", argv[0]);
        }
    }
#ifdef __DEBUG__
    pthread_setname_np(pthread_self(), "thread-main");
#endif
    tcp_server server(svr_port, thread_num);
    server.startup();
}