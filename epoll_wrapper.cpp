//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "epoll_wrapper.h"


int epoll_wrapper::add_event(int fd, int event, epoll_wrapper::epoll_event_callback callback) {
    struct epoll_event ev{};
    ev.events = event | EPOLLET;
    ev.data.fd = fd;
    int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
    if (perror_) {
        check_print(res, "epoll_ctl: ADD failed!\n");
    }
    callbacks_.insert(std::make_pair(fd, std::move(callback)));
    events_.insert(std::make_pair(fd, ev));
    return 0;
}

int epoll_wrapper::modify_event(int fd, int event) {
    if (callbacks_.find(fd) == callbacks_.end() ||
        events_.find(fd) == events_.end()) {
        return -1;
    }
    events_.at(fd).events = event | EPOLLET;
    int res = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &events_.at(fd));
    if (perror_) {
        check_print(res, "epoll_ctl: MOD failed!");
        return -1;
    }
    return 0;
}

int epoll_wrapper::delete_event(int fd) {
    if (callbacks_.find(fd) == callbacks_.end() ||
        events_.find(fd) == events_.end()) {
        return -1;
    }
    int res = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
    if (perror_) {
        check_print(res, "epoll_ctl: DEL failed!");
        return -1;
    }
    callbacks_.erase(fd);
    events_.erase(fd);
    return 0;
}

void epoll_wrapper::poll_event(int timeout = 0) {
    int readyn = epoll_wait(epoll_fd_, evs_poll, MAX_EVENTS, timeout);
    if (perror_) {
        check_print(readyn, "epoll_wait: error");
    }
    if (readyn == -1) {
        return;
    }
    // 获取所有的事件列表
    for (int i = 0; i < readyn; ++i) {
        struct epoll_event &curr_ev = evs_poll[i];
        if (callbacks_.find(curr_ev.data.fd) != callbacks_.end()) {
            callbacks_.at(curr_ev.data.fd)(curr_ev);
        }
    }
}
