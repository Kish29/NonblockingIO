//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "utils.h"
#include "fcntl.h"

void check_print_abt(int res, const char *msg, bool cr) {
    check_print(res, msg, cr);
    if (res == -1) {
        abort();
    }
}

void check_print(int res, const char *msg, bool cr) {
    if (res == -1) {
        if (cr) {
            fprintf(stderr, "%s\n", msg);
        } else {
            fprintf(stderr, "%s", msg);
        }
        if (errmap.find(errno) != errmap.end()) {
            fprintf(stderr, "%s\n", errmap[errno].c_str());
        }
    }
}

int set_fd_non_block(int fd) {
    int flag = fcntl(fd, F_GETFL);
    if (flag == -1) return -1;
    flag |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag);
}

void check_print_abt(int res, const char *msg, const char *func, int line, bool cr) {
    check_print(res, msg, func, line, cr);
    if (res == -1) {
        abort();
    }
}

void check_print(int res, const char *msg, const char *func, int line, bool cr) {
    if (res == -1) {
        if (cr) {
            fprintf(stderr, "%s %s.%d\n", msg, func, line);
        } else {
            fprintf(stderr, "%s %s.%d", msg, func, line);
        }
        if (errmap.find(errno) != errmap.end()) {
            fprintf(stderr, "%s\n", errmap[errno].c_str());
        }
    }
}
