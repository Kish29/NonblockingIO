//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_EPOLL_WRAPPER_H
#define EPOLLLEARN_EPOLL_WRAPPER_H

#include "unistd.h"
#include "vector"
#include "sys/epoll.h"
#include "fcntl.h"
#include "cstdio"
#include "utils.h"
#include "functional"

class epoll_wrapper {
public:
    typedef std::function<void(const struct epoll_event &)> EpollEventCallback;

    explicit epoll_wrapper(bool _perror = true) : perror_(_perror) {
        epoll_fd_ = epoll_create1(O_CLOEXEC);
        if (perror_) {
            check_print_abt(epoll_fd_, "epoll_create() failed!");
        }
        int res = fcntl(epoll_fd_, F_SETFL, O_NONBLOCK);
        if (perror_) {
            check_print(res, "fcntl: set O_NONBLOCK failed!");
        }
    }

    ~epoll_wrapper() {
        close(epoll_fd_);
    }

    // fd不能是普通文件的描述符或者目录的描述符
    // 可以是管道、消息队列、FIFO、socket、终端、epoll
    int add_event(int fd, int event, EpollEventCallback callback);

    int modify_event(int fd, int event);

    int delete_event(int fd);

    void poll_event(int timeout);

private:
    static const uint32_t MAX_EVENTS = 1024;

    bool perror_{};
    int epoll_fd_{};
    std::map<int, epoll_event> events_{};
    std::map<int, EpollEventCallback> callbacks_{};
    struct epoll_event evs_poll[MAX_EVENTS]{};
};


#endif //EPOLLLEARN_EPOLL_WRAPPER_H
