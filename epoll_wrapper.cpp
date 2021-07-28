//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include <cstdint>
#include "epoll_wrapper.h"


int epoll_wrapper::add_event(int fd, uint32_t event, bool ignore_err) {
    struct epoll_event ev{};
    ev.events = event | EPOLLET;
    ev.data.fd = fd;
    int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
    if (!ignore_err) {
        return 0;
    }
    CHECK_PERROR(res, "epoll_ctl: ADD failed!")
    return res;
}


int epoll_wrapper::modify_event(int fd, int event) {
    // 使用 EPOLL_CTL_ADD 可以更快地修改fd
    return add_event(fd, event, true);
}

int epoll_wrapper::delete_event(int fd) const {
    int res = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
    CHECK_PERROR(res, "epoll_ctl: DEL failed!")
    return res;
}

void epoll_wrapper::poll_event(int timeout = 0) {
    int readyn = epoll_wait(epoll_fd_, evary_for_poll, MAX_EVENTS, timeout);
    CHECK_PERROR(readyn, "epoll_wait: error")
    if (readyn == -1) {
        return;
    }
    callback_(evary_for_poll, readyn);
}

