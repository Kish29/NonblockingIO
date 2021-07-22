cmake_minimum_required(VERSION 3.19)
project(EpollLearn)

set(CMAKE_CXX_STANDARD 14)

set(
        SRCS
        Main.cpp
        AsyncSigIO.cpp
        CBreakTerm.cpp
        Epoll.cpp
        ErrorMap.cpp
        Utils.cpp
)

set(CMAKE_CXX_FLAGS -lpthread)

add_executable(EpollLearn ${SRCS})