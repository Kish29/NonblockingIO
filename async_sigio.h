//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_ASYNC_SIGIO_H
#define EPOLLLEARN_ASYNC_SIGIO_H

#include "unistd.h"
#include "functional"
#include "csignal"
#include "map"

class async_sigio {
public:
    typedef std::function<void(int)> sig_callback;

    async_sigio(int _fd, sig_callback _callback, bool _perror = false)
            : perror_(_perror),
              async_fd_(_fd),
              callback_(std::move(_callback)) {
        init_reg_callback();
    }

    ~async_sigio() {
        // 从callbacks中删除该类的callback
        close(async_fd_);
    }

    int enable();

private:

    bool perror_{};
    int async_fd_{};
    struct sigaction sa_{};
    sig_callback callback_{};

    void init_reg_callback();

    static std::multimap<int, sig_callback> callbacks_;

    static void async_sigio_handler(int sig);
};


#endif //EPOLLLEARN_ASYNC_SIGIO_H
