//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_UTILS_H
#define EPOLLLEARN_UTILS_H

#include "error_map.h"

#define CHECK_PERROR(ck, msg) \
if (ck == -1) {               \
    perror(msg);              \
}

#define CHECK_PERROR_ABT(ck, msg)   \
if (ck == -1) {                     \
    perror(msg);                    \
    abort();                        \
}

void check_print_abt(int res, const char *msg, bool cr = true);

void check_print_abt(int res, const char *msg, const char *func, int line, bool cr = true);

void check_print(int res, const char *msg, bool cr = true);

void check_print(int res, const char *msg, const char *func, int line, bool cr = true);

int set_fd_non_block(int fd);

#endif //EPOLLLEARN_UTILS_H
