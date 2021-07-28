cmake_minimum_required(VERSION 3.19)
project(EpollLearn)

set(CMAKE_CXX_STANDARD 14)

set(
        SRCS
        async_sigio.cpp
        cbreak_term.cpp
        epoll_wrapper.cpp
        error_map.cpp
        utils.cpp
        httpcode_map.cpp
        tcp_server.cpp
        tcp_client.cpp
        tcp_worker_pool.cpp
        http_parser.cpp
        thread_pool.cpp
        no_copyable.h
)

add_library(libepoll ${SRCS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3 -g -Wall -D__DEBUG__")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3 -g -Wall")

macro(add_example_target TEST_SRC)
    add_executable(example_${TEST_SRC} example_${TEST_SRC}.cpp)
    target_link_libraries(example_${TEST_SRC} libepoll pthread)
endmacro()

add_executable(EpollLearn main.cpp)
target_link_libraries(EpollLearn libepoll)

add_example_target(tcpserver)
add_example_target(tcpclient)
