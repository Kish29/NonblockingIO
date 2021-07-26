//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_EPOLL_WRAPPER_H
#define EPOLLLEARN_EPOLL_WRAPPER_H

#include "memory"
#include "unordered_set"
#include "unistd.h"
#include "vector"
#include "sys/epoll.h"
#include "fcntl.h"
#include "cstdio"
#include "utils.h"
#include "functional"

static const uint32_t EPOLL_WAIT_TIME = 10000;

class epoll_wrapper {
public:
    typedef std::function<void(const struct epoll_event *, int size)> event_callback_func;

    explicit epoll_wrapper(const event_callback_func &callback) {
        epoll_fd_ = epoll_create1(O_CLOEXEC);
        CHECK_PERROR_ABT(epoll_fd_, "epoll_create() failed!")
        int res = fcntl(epoll_fd_, F_SETFL, O_NONBLOCK);
        CHECK_PERROR(res, "fcntl: set O_NONBLOCK failed!")
        callback_ = callback;
    }

    ~epoll_wrapper() {
        close(epoll_fd_);
        for (const int &fd: fds_) {
            close(fd);
        }
    }

    // fd不能是普通文件的描述符或者目录的描述符
    // 可以是管道、消息队列、FIFO、socket、终端、epoll
    int add_event(int fd, int event, bool ignore_err = false);

    int modify_event(int fd, int event);

    int delete_event(int fd);

    void poll_event(int timeout);

    void set_callback(event_callback_func callback) {
        callback_ = std::move(callback);
    }

private:
    static const uint32_t MAX_EVENTS = 1024;

    int epoll_fd_{};
    event_callback_func callback_{};
    std::unordered_set<int> fds_{};
    epoll_event evary_for_poll[MAX_EVENTS]{};
};


#endif //EPOLLLEARN_EPOLL_WRAPPER_H
