#include "async_sigio.h"
#include "error_map.h"
#include "cbreak_term.h"
#include "fcntl.h"
#include "sys/epoll.h"
#include "epoll_wrapper.h"
#include "utils.h"
#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "pthread.h"

/******************** signal drive IO test ****************/
int targetfd = STDIN_FILENO;

volatile bool finish = false;

void sigio_handler(int sig) {
    char ch;
    printf("io comes in!\n");
    while (read(targetfd, &ch, 1) > 0 && !finish) {
        if (ch != '\r' || ch != '\n') {
            printf("read %c\n", ch);
        } else /*if (ch == '\r')*/ {
            printf("read <CR>");
        }
        finish = ch == '#';
    }
}

/*std::string event_str(uint32_t events) {
    std::string info{};
    if (events | EPOLLIN) {
        info += "epoll in ";
    }
    if (events | EPOLLHUP) {
        info += "epoll hup ";
    }
    if (events | EPOLLERR) {
        info += "epoll err ";
    }
    return info;
}*/

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
    return str;
}

void o_async_io() {
    async_sigio asyncSigIo(targetfd, sigio_handler, true);
    asyncSigIo.enable();
    cbreak_term(targetfd, true);
    while (!finish) {
        printf("do other things before io comes in\n");
        sleep(2);
    }
}


/*************************** epoll test ****************************/
volatile int open_fd = 0;

void read_event(const struct epoll_event &curr_ev) {
    printf("fd=%d, events: %s\n", curr_ev.data.fd, event_str(curr_ev.events));
    if (curr_ev.events & EPOLLIN) {
        char buf[255];
        size_t readn = read(curr_ev.data.fd, buf, 255);
        check_print_abt((int) readn, "read error!");
        printf("read %lu bytes: %s\n", readn, buf);
    } else if (curr_ev.events & (EPOLLHUP | EPOLLERR)) {
        open_fd--;
    }
}

void *thread_poll(void *arg) {
    printf("在子线程中轮询...每2s轮询一次\n");
    epoll_wrapper epoll = *((epoll_wrapper *) arg);
    while (open_fd > 0) {
        printf("epoll轮询...\n");
        epoll.poll_event(0);
        sleep(2);
    }
    printf("open_fd all closed!\n");
    pthread_exit(nullptr);
}

void epoll_pipe(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return;
    }
    epoll_wrapper epoll;
    int epoll_fd = epoll_create1(O_CLOEXEC);
    check_print_abt(epoll_fd, "epoll_create failed!");

    // open arguments
    for (int i = 1; i < argc; ++i) {
        int fd = open(argv[i], O_RDONLY);
        check_print_abt(fd, "open file descriptor failed!");
        printf("open %s on %d\n", argv[i], fd);
        // 有数据输入，边缘触发
        epoll.add_event(fd, EPOLLIN | EPOLLET, read_event);
    }
    open_fd = argc - 1;
    pthread_t t{};
    pthread_create(&t, nullptr, thread_poll, (epoll_wrapper *) &epoll);
    pthread_join(t, nullptr);
}

int main(int argc, char *argv[]) {
//    o_async_io();
    epoll_pipe(argc, argv);
    exit(EXIT_SUCCESS);
}
