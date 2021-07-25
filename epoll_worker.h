//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_EPOLL_WORKER_H
#define EPOLLLEARN_EPOLL_WORKER_H

#include "epoll_wrapper.h"

class epoll_worker {

public:

    void start();

    void join();

private:
    epoll_wrapper poller_{};
    pthread_t poll_thread_{};

};


#endif //EPOLLLEARN_EPOLL_WORKER_H
