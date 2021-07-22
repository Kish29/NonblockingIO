//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "Utils.h"

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
        if (error_map.find(errno) != error_map.end()) {
            fprintf(stderr, "%s\n", error_map[errno].c_str());
        }
    }
}
