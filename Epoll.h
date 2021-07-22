//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_EPOLL_H
#define EPOLLLEARN_EPOLL_H

#include "vector"
#include "sys/epoll.h"
#include "fcntl.h"
#include "cstdio"
#include "Utils.h"
#include "functional"

class Epoll {
public:
    typedef std::function<void(const struct epoll_event &)> EpollEventCallback;

    explicit Epoll(bool _perror = true) : perror_(_perror) {
        epollfd_ = epoll_create1(O_CLOEXEC);
        if (perror_) {
            check_print_abt(epollfd_, "epoll_create() failed!");
        }
        int res = fcntl(epollfd_, F_SETFL, O_NONBLOCK);
        if (perror_) {
            check_print(res, "fcntl: set O_NONBLOCK failed!");
        }
    }

    // fd不能是普通文件的描述符或者目录的描述符
    // 可以是管道、消息队列、FIFO、socket、终端、epoll
    int addEvent(int fd, int event, EpollEventCallback callback);

    int modifyEvent(int fd, int event);

    int deleteEvent(int fd);

    void poll(int timeout);

private:
    static const uint32_t MAX_EVENTS = 1024;

    bool perror_{};
    int epollfd_{};
    std::map<int, epoll_event> events_{};
    std::map<int, EpollEventCallback> callbacks_{};
    struct epoll_event evs_poll[MAX_EVENTS]{};
};


#endif //EPOLLLEARN_EPOLL_H
