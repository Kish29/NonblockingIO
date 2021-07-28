//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#include "tcp_worker_pool.h"
#include "sys/socket.h"
#include "cstring"
#include "http_parser.h"

using std::to_string;

#define DISCONNECT(target) close(target)

char *event_str(uint32_t events) {
    char *str = new char[128];
    char *p = str;
    if (events & EPOLLIN) {
        strcpy(p, "epoll in ");
    }
    p += strlen("epoll in ");
    if (events & EPOLLHUP) {
        strcpy(p, "epoll hup ");
    }
    p += strlen("epoll hup ");
    if (events & EPOLLERR) {
        strcpy(p, "epoll err ");
    }
    p += strlen("epoll err");
    if (events & EPOLLRDHUP) {
        strcpy(p, "epoll hup");
    }
    return str;
}

__thread char TRD_SEND_BUF[BUFSIZ]{};
__thread char TRD_RECV_BUF[BUFSIZ]{};

tcp_worker::tcp_worker(tcp_worker_pool *mst) : poller_([&](const epoll_event *events, int size) -> void {
    for (int i = 0; i < size; ++i) {
        int client_sockfd = events[i].data.fd;
#ifdef __DEBUG__
        if (client_infos.find(client_sockfd) != client_infos.end()) {
            char buf[64];
            pthread_getname_np(pthread_self(), buf, 64);
            printf("%s: new epoll event from %s:%d and events is %s\n",
                   buf,
                   client_infos[client_sockfd].host,
                   client_infos[client_sockfd].port,
                   event_str(events[i].events)
            );
        }
#endif
// 注意，客户端断开连接时，会先到来一个信号EPOLLIN，然后才是EPOLLHUP ｜ EPOLLERR
        if (events[i].events & EPOLLIN) {
            handle_epollin(client_sockfd);
        }
        if (events[i].events & (EPOLLHUP | EPOLLERR)) {
            handle_disconnect(client_sockfd);
        }
    }
}), master_(mst) {}


void tcp_worker::handle_epollin(int client_sockfd) {
    ssize_t n = read(client_sockfd, TRD_RECV_BUF, BUFSIZ);
    // 判断客户端是否已经断开连接
    if (n == 0) {
        handle_disconnect(client_sockfd);
        return;
    }
    // 读取错误
    if (n < 0) {
        handle_error(400, client_sockfd);
        return;
    }
    // 向客户端发送数据
    sprintf(TRD_SEND_BUF, "%s %d %s%s", HTTP_VERSION, 200, http_codes.at(200).c_str(), CRLF);
    sprintf(TRD_SEND_BUF + strlen(TRD_SEND_BUF), "Content-length: %d%s%s", 0, CRLF, CRLF);
    send(client_sockfd, TRD_SEND_BUF, BUFSIZ, MSG_NOSIGNAL);
}

void tcp_worker::handle_disconnect(int client_sockfd) {
    // 不需要加锁
    if (client_infos.find(client_sockfd) != client_infos.end()) {
        printf("Connection from %s:%d disconnected\n",
               client_infos[client_sockfd].host,
               client_infos[client_sockfd].port);
        client_infos.erase(client_sockfd);
    }
    poller_.delete_event(client_sockfd);
    // 关闭连接
    DISCONNECT(client_sockfd);
}

void tcp_worker::handle_error(int errcode, int client_sockfd) {
    sprintf(TRD_SEND_BUF, "HTTP/1.1 %d %s\r\n", errcode, http_codes.at(errcode).c_str());
    send(client_sockfd, TRD_SEND_BUF, BUFSIZ, MSG_NOSIGNAL);
}

void tcp_worker::start() {
    if (!started_) {
        pthread_create(&thread_id_, nullptr, worker_routine, (void *) this);
        started_ = true;
    } else {}
}

void *tcp_worker::worker_routine(void *instance) {
    auto *inst = (tcp_worker *) instance;
    if (inst && inst->master_) {
        // 设置cpu亲和性
        cpu_set_t mask{};
        CPU_ZERO(&mask);
        CPU_SET(inst->cpu_affinity_id_, &mask);
        // getpid获取的是进程id，同一进程下的不同线程pid是一样的
        // 要获取线程id，请使用tid
//        int res = sched_setaffinity(getpid(), sizeof mask, &mask);
        // 正确设置线程cpu核心亲和力的有下面三种方式
        int res = sched_setaffinity(gettid(), sizeof mask, &mask);
//        int res = sched_setaffinity(0, sizeof mask, &mask);
//        int res = pthread_setaffinity_np(pthread_self(), sizeof mask, &mask);
        CHECK_PERROR(res, TRD_SEND_BUF)
#ifdef __DEBUG__
        res = pthread_setname_np(pthread_self(), inst->name_.c_str());
        CHECK_PERROR(res, "pthread_setname_np failed!")
#endif

        while (true) {
            if (!inst->master_->event_storage_.empty()) {
                {
                    worker_lockguard lck(inst->master_->locker_);
                    if (!inst->master_->event_storage_.empty()) {
#ifdef __DEBUG__
                        inst->print_info("queue is not empty now\nworker poller add epoll event\n");
#endif
                        // 从队列中取出新连接的客户端
                        struct client_info info = inst->master_->event_storage_.front();
                        inst->master_->event_storage_.pop();

                        // 记录客户端信息
                        inst->client_infos.insert(std::make_pair(info.eplev.data.fd, info));
                        // 添加到epoll中
                        inst->poller_.add_event(info.eplev.data.fd, info.eplev.events);
                    }
                    /*if () {
                        inst->print_info("queue is empty, wait...\n");
                        inst->master_->condition_.wait_for(1);
                        inst->master_->condition_.wait();
                    }*/
                }
            }
#ifdef __DEBUG__
            inst->print_info("worker do poll\n");
#endif
            inst->master_->avail_woker_num_--;
            // 为了达到最快的响应，使用求模算法，根据总的线程数mod可用线程数可以计算出合理的等待时长
            // 我发明的！^_^ !
            int wttme{};
            if (inst->master_->avail_woker_num_ > inst->master_->worker_num_ / 2) {
                wttme = inst->master_->avail_woker_num_;
            } else {
                wttme = inst->master_->worker_num_ % inst->master_->avail_woker_num_;
            }
            wttme = (wttme == 0 ? 1 : wttme);
            inst->poller_.poll_event(wttme);
            inst->master_->avail_woker_num_++;
        }
    }
//    return nullptr;
}

void tcp_worker::join() {

}

void tcp_worker::stop() {

}


tcp_worker_pool::tcp_worker_pool(uint16_t worker_num)
        : worker_num_(
        COERCE_IN(CPU_CORE, worker_num, CPU_CORE * 12)) {
    // 构造函数禁止调用shared_from_this
    // 因为构造函数还没有执行完，类实例还未创建
#ifdef __DEBUG__
    printf("worker_num_ is %d\n and max threads num is %d\n", worker_num_, _SC_THREAD_THREADS_MAX);
#endif
    workers_ = new std::shared_ptr<tcp_worker>[worker_num_];
    for (int i = 0; i < worker_num_; ++i) {
//        workers_[i] = std::make_shared<tcp_worker>(shared_from_this());
        workers_[i] = std::make_shared<tcp_worker>(this);
        workers_[i]->name_ = std::string("worker-thread-") + std::to_string(i);
        workers_[i]->cpu_affinity_id_ = i % CPU_CORE;
    }
}

void tcp_worker_pool::startup() {
    if (workers_) {
        for (int i = 0; i < worker_num_; ++i) {
            if (workers_[i]) {
                workers_[i]->start();
            }
        }
    }
    avail_woker_num_ = worker_num_;
    // 初始化线程池
    thread_pool::instance()->create(CPU_CORE);
    started_ = true;
}

void tcp_worker_pool::submit(const client_info &info, bool nonblocking) {
    if (!started_) {
        startup();
    }
    thread_pool::instance()->submit(nullptr, [&, info, nonblocking](const std::shared_ptr<void> &args) -> void {
        if (nonblocking) {
            // 查询非阻塞
            set_fd_non_block(info.eplev.data.fd);
        }
        // 执行http解析
        auto body = std::string("<!DOCTYPE html>\n")
                    +
                    "<!--STATUS OK-->\n"
                    +
                    "<html>\n"
                    +
                    "<head>\n"
                    +
                    "    <meta http-equiv=content-type content=text/html;charset=utf-8>\n"
                    +
                    "    <meta http-equiv=X-UA-Compatible content=IE=Edge>\n"
                    +
                    "    <meta content=always name=referrer>\n"
                    +
                    "    <link rel=stylesheet type=text/css href=http://s1.bdstatic.com/r/www/cache/bdorz/baidu.min.css>\n"
                    +
                    "    <title>ç\u0099¾åº¦ä¸\u0080ä¸\u008Bï¼\u008Cä½ å°±ç\u009F¥é\u0081\u0093</title></head>\n"
                    +
                    "<body link=#0000cc>\n"
                    +
                    "<div id=wrapper>\n"
                    +
                    "    <div id=head>\n"
                    +
                    "        <div class=head_wrapper>\n"
                    +
                    "            <div class=s_form>\n"
                    +
                    "                <div class=s_form_wrapper>\n"
                    +
                    "                    <div id=lg><img hidefocus=true src=//www.baidu.com/img/bd_logo1.png width=270 height=129></div>\n"
                    +
                    "                    <form id=form name=f action=//www.baidu.com/s class=fm><input type=hidden name=bdorz_come value=1>\n"
                    +
                    "                        <input type=hidden name=ie value=utf-8> <input type=hidden name=f value=8> <input type=hidden\n"
                    +
                    "                                                                                                          name=rsv_bp\n"
                    +
                    "                                                                                                          value=1>\n"
                    +
                    "                        <input type=hidden name=rsv_idx value=1> <input type=hidden name=tn value=baidu><span\n"
                    +
                    "                                class=\"bg s_ipt_wr\"><input id=kw name=wd class=s_ipt value maxlength=255\n"
                    +
                    "                                                           autocomplete=off autofocus></span><span\n"
                    +
                    "                                class=\"bg s_btn_wr\"><input type=submit id=su value=ç\u0099¾åº¦ä¸\u0080ä¸\u008B class=\"bg s_btn\"></span>\n"
                    +
                    "                    </form>\n"
                    +
                    "                </div>\n"
                    +
                    "            </div>\n"
                    +
                    "            <div id=u1><a href=http://news.baidu.com name=tj_trnews class=mnav>æ\u0096°é\u0097»</a> <a href=http://www.hao123.com\n"
                    +
                    "                                                                                             name=tj_trhao123\n"
                    +
                    "                                                                                             class=mnav>hao123</a> <a\n"
                    +
                    "                    href=http://map.baidu.com name=tj_trmap class=mnav>å\u009C°å\u009B¾</a> <a href=http://v.baidu.com\n"
                    +
                    "                                                                                     name=tj_trvideo\n"
                    +
                    "                                                                                     class=mnav>è§\u0086é¢\u0091</a> <a\n"
                    +
                    "                    href=http://tieba.baidu.com name=tj_trtieba class=mnav>è´´å\u0090§</a>\n"
                    +
                    "                <noscript><a\n"
                    +
                    "                        href=http://www.baidu.com/bdorz/login.gif?login&amp;tpl=mn&amp;u=http%3A%2F%2Fwww.baidu.com%2f%3fbdorz_come%3d1\n"
                    +
                    "                        name=tj_login class=lb>ç\u0099»å½\u0095</a></noscript>\n"
                    +
                    "                <script>document.write('<a href=\"http://www.baidu.com/bdorz/login.gif?login&tpl=mn&u=' + encodeURIComponent(window.location.href + (window.location.search === \"\" ? \"?\" : \"&\") + \"bdorz_come=1\") + '\" name=\"tj_login\" class=\"lb\">ç\u0099»å½\u0095</a>');</script>\n"
                    +
                    "                <a href=//www.baidu.com/more/ name=tj_briicon class=bri style=\"display: block;\">æ\u009B´å¤\u009Aäº§å\u0093\u0081</a></div>\n"
                    +
                    "        </div>\n"
                    +
                    "    </div>\n"
                    +
                    "    <div id=ftCon>\n"
                    +
                    "        <div id=ftConw><p id=lh><a href=http://home.baidu.com>å\u0085³äº\u008Eç\u0099¾åº¦</a> <a href=http://ir.baidu.com>About\n"
                    +
                    "            Baidu</a></p>\n"
                    +
                    "            <p id=cp>&copy;2017&nbsp;Baidu&nbsp;<a href=http://www.baidu.com/duty/>ä½¿ç\u0094¨ç\u0099¾åº¦å\u0089\u008Då¿\u0085è¯»</a>&nbsp; <a\n"
                    +
                    "                    href=http://jianyi.baidu.com/ class=cp-feedback>æ\u0084\u008Fè§\u0081å\u008F\u008Dé¦\u0088</a>&nbsp;äº¬ICPè¯\u0081030173å\u008F·&nbsp; <img\n"
                    +
                    "                    src=//www.baidu.com/img/gs.gif></p></div>\n"
                    +
                    "    </div>\n"
                    +
                    "</div>\n"
                    +
                    "</body>\n"
                    +
                    "</html>";
        ssize_t readn = read(info.eplev.data.fd, TRD_RECV_BUF, BUFSIZ);
        if (readn > 0) {
            printf("and http request is: %s\n", TRD_RECV_BUF);
            sprintf(TRD_SEND_BUF, "HTTP/1.1 200 OK\r\nContent-length: %zu\r\n\r\n", strlen(body.c_str()));
            auto p = TRD_SEND_BUF + strlen(TRD_SEND_BUF);
            strcpy(p, body.c_str());
            send(info.eplev.data.fd, TRD_SEND_BUF, BUFSIZ, MSG_NOSIGNAL);
        } else if (readn == 0) {
            strcpy(TRD_SEND_BUF, "i will keep you connected\n");
            send(info.eplev.data.fd, TRD_SEND_BUF, BUFSIZ, MSG_NOSIGNAL);
        } else {
            perror("accept read error occurred!");
        }
        // 提交到队列中
        {
            // todo：需要检查队列的堆积
            worker_lockguard lck(locker_);
#ifdef __DEBUG__
            printf("put new obs into queue\n");
#endif
            event_storage_.emplace(info);
//        printf("notify one\n");
//        condition_.notify_one();
        }
    });
}