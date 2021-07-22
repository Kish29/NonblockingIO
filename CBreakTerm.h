//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_TERMUTIL_H
#define EPOLLLEARN_TERMUTIL_H

#include "termios.h"

// cbreak模式的终端，每次输入仅一个字符
class CBreakTerm {
public:
    explicit CBreakTerm(int _infd, bool _perror = false) : perror_(_perror), infd_(_infd) {
        init();
    }

private:
    bool perror_{};
    int infd_{};
    struct termios tmio_{};

    int init();
};


#endif //EPOLLLEARN_TERMUTIL_H
