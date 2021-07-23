//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "utils.h"

void check_print_abt(int res, const char *msg, bool line) {
    check_print(res, msg, line);
    if (res == -1) {
        abort();
    }
}

void check_print(int res, const char *msg, bool line) {
    if (res == -1) {
        if (line) {
            fprintf(stderr, "%s\n", msg);
        } else {
            fprintf(stderr, "%s", msg);
        }
        if (errmap.find(errno) != errmap.end()) {
            fprintf(stderr, "%s\n", errmap[errno].c_str());
        }
    }
}
