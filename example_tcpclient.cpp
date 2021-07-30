//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//
#include "fcntl.h"
#include "unistd.h"
#include "iostream"
#include "no_copyable.h"
#include "sys/socket.h"
#include "tcp_client.h"

void client_test(int argc, char *argv[]);

void other_test();

void *thread_routine(void *) {
    printf("thread pid is %d\n", gettid());
    pthread_exit(nullptr);
}

void pthread_pid_test() {
    pthread_t ids[4];
    for (int i = 0; i < 4; ++i) {
        pthread_create(&ids[i], nullptr, thread_routine, nullptr);
        pthread_join(ids[i], nullptr);
    }
}

int main(int argc, char *argv[]) {
    client_test(argc, argv);
//    other_test();
//    printf("father pid is %d\n", gettid());
//    pthread_pid_test();
    return 0;
}


void client_test(int argc, char *argv[]) {
    if (argc < 3) {
        hint:
        printf("usage %s -s [server_address] -p [server_port]", argv[0]);
        exit(EXIT_SUCCESS);
    }
    const char *pattern = "s:p:";
    const char *svr_addr{};
    uint16_t svr_port{};
    int opt{};
    while ((opt = getopt(argc, argv, pattern)) != -1) {
        switch (opt) {
            case 's': {
                svr_addr = optarg;
                break;
            }
            case 'p': {
                svr_port = (uint16_t) atoi(optarg);
                break;
            }
            default:
                goto hint;
        }
    }
    tcp_client client{};
    client.cnnto_server2(svr_port, svr_addr);
//    while (true);
}

class A {
public:
    A() {
        printf("A constructor\n");
    }

    virtual ~A() = 0;
};

// 纯虚析构函数必须有实现，因为最后要调用
A::~A() {
    printf("A destructor\n");
}

class B : A {
public:
    explicit B(const char *name) : name_(name) {
        printf("B constructor %s\n", name_);
    }

    ~B() override {
        printf("B destructor %s\n", name_);
    }

private:
    const char *name_{};
};

class C : no_copyable {
public:
    explicit C(int a) {
        printf("C constructor a is %d\n", a);
    }
};

#include "queue"
#include "memory"

struct ss {
    int a{};
};

void other_test() {
//    B b{"Bob"};
//    B b2{"Jane"};
//    C c(19);
    typedef std::shared_ptr<ss> sp_ss;
    std::queue<sp_ss> xx{};
    for (int i = 0; i < 10; ++i) {
        auto m = std::make_shared<ss>();
        xx.emplace(m);
    }
}
