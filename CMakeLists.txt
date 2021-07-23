cmake_minimum_required(VERSION 3.19)
project(EpollLearn)

set(CMAKE_CXX_STANDARD 14)

set(
        SRCS
        main.cpp
        async_sigio.cpp
        cbreak_term.cpp
        epoll_wrapper.cpp
        error_map.cpp
        utils.cpp
)

set(CMAKE_CXX_FLAGS -lpthread)

add_executable(EpollLearn ${SRCS})