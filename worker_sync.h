//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// 互斥锁、同步
//

#ifndef EPOLLLEARN_WORKER_SYNC_H
#define EPOLLLEARN_WORKER_SYNC_H

#include "ctime"
#include "cerrno"
#include "cstdint"
#include "no_copyable.h"
#include "pthread.h"

class worker_lock;

class worker_cond;

class worker_lockguard;

class worker_lock : no_copyable {

public:
    worker_lock() {
        pthread_mutex_init(&mutex_, nullptr);
    }

    ~worker_lock() override {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }

    void lock() {
        pthread_mutex_lock(&mutex_);
    }

    void unlock() {
        pthread_mutex_unlock(&mutex_);
    }

private:
    pthread_mutex_t mutex_{};

private:
    friend class worker_cond;
};

class worker_lockguard : no_copyable {
public:
    explicit worker_lockguard(worker_lock &_locker) : locker_(_locker) {
        locker_.lock();
    }

    ~worker_lockguard() override {
        locker_.unlock();
    }

    void unlock() const {
        locker_.unlock();
    }

private:
    worker_lock &locker_;
};

class worker_cond : no_copyable {
public:

    explicit worker_cond(worker_lock &_locker) : locker_(_locker) {
        pthread_cond_init(&cond_, nullptr);
    }

    void notify_one() {
        pthread_cond_signal(&cond_);
    }

    void notify_all() {
        pthread_cond_broadcast(&cond_);
    }

    void wait() {
        pthread_cond_wait(&cond_, &(locker_.mutex_));
    }

    bool wait_for(uint32_t seconds) {
        timespec time{};
        clock_gettime(CLOCK_REALTIME, &time);
        // 设定超时时间点
        time.tv_sec += static_cast<time_t>(seconds);
        // 返回值为true表示超时了获取了锁
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, &(locker_.mutex_), &time);
    }

private:
    worker_lock &locker_;
    pthread_cond_t cond_{};

};

#endif //EPOLLLEARN_WORKER_SYNC_H
