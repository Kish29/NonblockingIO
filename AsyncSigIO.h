//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_ASYNCSIGIO_H
#define EPOLLLEARN_ASYNCSIGIO_H

#include <utility>

#include "unistd.h"
#include "functional"
#include "csignal"
#include "map"

class AsyncSigIO {
public:
    typedef std::function<void(int)> Callback;

    AsyncSigIO(int _fd, Callback _callback, bool _perror = false)
            : perror_(_perror),
              asyncfd_(_fd),
              callback_(std::move(_callback)) {
        initAndRegCallback();
    }

    ~AsyncSigIO() {
        // 从callbacks中删除该类的callback
        close(asyncfd_);
    }

    int enable();

private:

    bool perror_{};
    int asyncfd_{};
    struct sigaction sa_{};
    Callback callback_{};

    void initAndRegCallback();

    static std::multimap<int, Callback> Callbacks_;

    static void SigioHandler(int sig);
};


#endif //EPOLLLEARN_ASYNCSIGIO_H
