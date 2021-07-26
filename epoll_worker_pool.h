//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_EPOLL_WORKER_POOL_H
#define EPOLLLEARN_EPOLL_WORKER_POOL_H

#include <memory>

#include "worker_sync.h"
#include "epoll_wrapper.h"
#include "vector"
#include "queue"
#include "string"
#include "memory"
#include <utility>
#include "thread"

class epoll_worker;

class epoll_worker_pool;

const static uint16_t CPU_CORE = std::thread::hardware_concurrency();

class epoll_worker : no_copyable {

public:

    epoll_worker(epoll_worker_pool *mst, const epoll_wrapper::event_callback_func &func)
            : poller_(func), master_(mst) {}

    ~epoll_worker() override {
        master_ = nullptr;
    }

    void start();

    void join();

    void stop();

    void set_timeout(int timeout) {
        poll_timeout_ = timeout;
    }

private:
    epoll_wrapper poller_;
    epoll_worker_pool *master_;
    pthread_t thread_id_{};
    bool started_{};
    int poll_timeout_{EPOLL_WAIT_TIME};

private:
    static void *worker_routine(void *instance);

private:
    friend class epoll_worker_pool;
};

class epoll_worker_pool : no_copyable {
public:

    explicit epoll_worker_pool(epoll_wrapper::event_callback_func func, uint16_t worker_num = CPU_CORE);

    void startup();

    // 提交一个新的连接事件
    void submit(int fd, int events, bool nonblocking = true);

    ~epoll_worker_pool() override {
        delete[] workers_;
    }

    void set_poll_timeout(int timeout) {
        for (int i = 0; i < worker_num_; ++i) {
            if (workers_[i]) {
                workers_[i]->set_timeout(timeout);
            }
        }
    }

private:
    typedef std::shared_ptr<epoll_worker> sp_worker;

    epoll_wrapper::event_callback_func callback_;
    uint16_t worker_num_{CPU_CORE};
    sp_worker *workers_{};
    std::queue<epoll_event> event_storage_{};
    worker_lock locker_{};
    worker_cond condition_{locker_};

private:
    friend class epoll_worker;
};


#endif //EPOLLLEARN_EPOLL_WORKER_POOL_H
