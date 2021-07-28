//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_TCP_WORKER_POOL_H
#define EPOLLLEARN_TCP_WORKER_POOL_H

#include "worker_sync.h"
#include "epoll_wrapper.h"
#include "thread_pool.h"
#include "vector"
#include "queue"
#include "string"
#include "memory"

class tcp_worker;

class tcp_worker_pool;

struct client_info {
    const char *host{};
    int port{};
    struct epoll_event eplev{};
};

class tcp_worker : no_copyable {

public:

    explicit tcp_worker(tcp_worker_pool *mst);

    ~tcp_worker() override {
        master_ = nullptr;
    }

    void start();

    void join();

    void stop();

    void set_timeout(int timeout) {
        poll_timeout_ = timeout;
    }

    void setname(const std::string &name) {
        name_ = name;
    }

    const std::string &name() const {
        return name_;
    }

    void print_info(const char *msg) {
        printf("%s: %s", name_.c_str(), msg);
    }

private:
    epoll_wrapper poller_;
    tcp_worker_pool *master_;
    std::string name_{};
    pthread_t thread_id_{};
    uint32_t cpu_affinity_id_{};
    bool started_{};
#ifdef __DEBUG__
    int poll_timeout_{EPOLL_WAIT_TIME};    // 5ms等待查询
#else
    int poll_timeout_{MIN_EPOLL_WAIT_TIME};    // 1ms等待查询
#endif
    std::map<int, client_info> client_infos{};

private:

    void handle_epollin(int client_sockfd);

    void handle_disconnect(int client_sockfd);

    static void handle_error(int errcode, int client_sockfd);

    [[noreturn]]static void *worker_routine(void *instance);

private:
    friend class tcp_worker_pool;
};

class tcp_worker_pool : no_copyable {
public:

    explicit tcp_worker_pool(uint16_t worker_num = CPU_CORE);

    void startup();

    // 提交一个新的连接事件
    void submit(const client_info &info, bool nonblocking = true);

    ~tcp_worker_pool() override {
        delete[] workers_;
    }

    void set_poll_timeout(int timeout) {
        timeout = timeout < MIN_EPOLL_WAIT_TIME ? MIN_EPOLL_WAIT_TIME : timeout;
        for (int i = 0; i < worker_num_; ++i) {
            if (workers_[i]) {
                workers_[i]->set_timeout(timeout);
            }
        }
    }

private:
    typedef std::shared_ptr <tcp_worker> sp_worker;

    int worker_num_{CPU_CORE};
    bool started_{};
    sp_worker *workers_{};
    std::queue <client_info> event_storage_{};
    worker_lock locker_{};
    worker_cond condition_{locker_};

    // 此时没有被epoll_wait阻塞的woker数量
    std::atomic_int avail_woker_num_{};

private:
    friend class tcp_worker;
};


#endif //EPOLLLEARN_TCP_WORKER_POOL_H
