//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "cbreak_term.h"
#include "cstdio"

int cbreak_term::init() {
    if (tcgetattr(infd_, &tmio_) == -1) {
        if (perror_) {
            fprintf(stderr, "tcgetattr(): get attributes failed\n");
        }
        return -1;
    }

    tmio_.c_lflag &= ~(ICANON | ECHO);
    tmio_.c_lflag |= ISIG;
    tmio_.c_lflag &= ~ICRNL;

    tmio_.c_cc[VMIN] = 1;   // character-at-a-time input
    tmio_.c_cc[VTIME] = 0;  // with blocking

    if (tcsetattr(infd_, TCSAFLUSH, &tmio_) == -1) {
        if (perror_) {
            fprintf(stderr, "tcsetattr(): set attributes failed\n");
        }
        return -1;
    }
    return 0;
}
