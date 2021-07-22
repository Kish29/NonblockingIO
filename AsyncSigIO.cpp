//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "AsyncSigIO.h"
#include "fcntl.h"
#include "cstdio"
#include "unistd.h"

std::multimap<int, AsyncSigIO::Callback> AsyncSigIO::Callbacks_{};

void AsyncSigIO::initAndRegCallback() {
    sigemptyset(&sa_.sa_mask);
    sa_.sa_flags = SA_RESTART;
    sa_.sa_handler = SigioHandler;
    Callbacks_.insert(std::make_pair(SIGIO, callback_));
}

void AsyncSigIO::SigioHandler(int sig) {
    auto it = Callbacks_.equal_range(SIGIO);
    if (it.first != Callbacks_.end()) {
        for (auto pr = it.first; pr != it.second; pr++) {
            pr->second(sig);
        }
    }
}

int AsyncSigIO::enable() {
    // 接受SIGIO的handler
    if (sigaction(SIGIO, &sa_, nullptr) == -1) {
        if (perror_) {
            fprintf(stderr, "sigaction(): SIGIO set failed\n");
        }
        return -1;
    }

    // 设置接受SIGIO为本程序
    // STDIN_FILENO 标准键盘输入
    // STDOUT_FILENO 标准屏幕输出
    // F_SETOWN set process id or process group id that will receive SIGIO asn SIGURG
    if (fcntl(asyncfd_, F_SETOWN, getpid()) == -1) {
        if (perror_) {
            fprintf(stderr, "fcntl(): F_SETOWN set failed\n");
        }
        return -1;
    }

    // 设置信号驱动IO和非阻塞式系统调用
    int flags = fcntl(asyncfd_, F_GETFL);
    // O_ASYNC ：开启信号驱动IO
    flags |= (O_ASYNC | O_NONBLOCK);
    if (fcntl(asyncfd_, F_SETFL, flags) == -1) {
        if (perror_) {
            fprintf(stderr, "fcntl(): (O_ASYNC | O_NONBLOCK) set failed\n");
        }
        return -1;
    }
    return 0;
}
