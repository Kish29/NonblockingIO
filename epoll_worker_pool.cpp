//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#include "epoll_worker_pool.h"

epoll_worker_pool::epoll_worker_pool(
        epoll_wrapper::event_callback_func func,
        uint16_t worker_num
) : callback_(std::move(func)), worker_num_(worker_num) {
    // 构造函数禁止调用shared_from_this
    // 因为构造函数还没有执行完，类实例还未创建
    printf("worker_num_ is %d\n", worker_num_);
    workers_ = new std::shared_ptr<epoll_worker>[worker_num_];
    for (int i = 0; i < worker_num_; ++i) {
//        workers_[i] = std::make_shared<epoll_worker>(shared_from_this());
        workers_[i] = std::make_shared<epoll_worker>(this, callback_);
    }
}

void epoll_worker_pool::startup() {
    if (workers_) {
        for (int i = 0; i < worker_num_; ++i) {
            if (workers_[i]) {
                workers_[i]->start();
            }
        }
    }
}

void epoll_worker_pool::submit(int fd, int events, bool nonblocking) {
    if (nonblocking) {
        // 查询非阻塞
        set_fd_non_block(fd);
    }
    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = events;
    // 提交到队列中
    {
        // todo：需要检查队列的堆积
        worker_lockguard lck(locker_);
        printf("put new obs into queue\n");
        event_storage_.emplace(ev);
        printf("notify one\n");
        condition_.notify_one();
    }

}

void epoll_worker::start() {
    if (!started_) {
        pthread_create(&thread_id_, nullptr, worker_routine, (void *) this);
        started_ = true;
    } else {}
}

void *epoll_worker::worker_routine(void *instance) {
    auto *inst = (epoll_worker *) instance;
    if (inst->master_) {
        while (true) {
            {
                worker_lockguard lck(inst->master_->locker_);
                if (!inst->master_->event_storage_.empty()) {
                    printf("queue is not empty\n");
                    auto ev = inst->master_->event_storage_.front();
                    inst->master_->event_storage_.pop();
                    inst->poller_.add_event(ev.data.fd, ev.events);
                } else {
                    printf("queue is empty, wait...\n");
                    inst->master_->condition_.wait_for(1);
//                    inst->master_->condition_.wait();
                }
            }
            printf("worker do poll\n");
            inst->poller_.poll_event(inst->poll_timeout_);
        }
    }
    return nullptr;
}

