# 1、wildcard : 扩展通配符
# 2、notdir ： 去除路径
# 3、patsubst ：替换通配符
MAIN_SOURCE := main.cpp example_tcpserver.cpp example_tcpclient.cpp
# 获取所有.cpp文件
SOURCE		:= $(wildcard *.cpp tests/*.cpp)
# 过滤掉Main.o
# (filter pattern, TEXT) 保留text中符合pattern的字符串
# (filter-out pattern, TEXT) 去掉text中符合pattern的字符串
override SOURCE := $(filter-out $(MAIN_SOURCE), $(SOURCE))
# 将所有的.cpp编译成.o文件
OBJECTS 	:= $(patsubst %.cpp, %.o, $(SOURCE))

TARGET		:= epoll_learn
TEST_TARGET1	:= example_tcpserver
TEST_TARGET2	:= example_tcpclient
CXX			:= g++
LIBS		:= -lpthread
# -D宏定义__DEBUG__，控制编译分支
CFLAGS		:= -std=c++11 -g -Wall -O3 -D__DEBUG__
CXXFLAGS	:= $(CFLAGS)

.PHONY	: all clean

all	: $(TARGET) $(TEST_TARGET1) $(TEST_TARGET2)


$(TARGET) : $(OBJECTS) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(TEST_TARGET1) : $(OBJECTS) example_tcpserver.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(TEST_TARGET2) : $(OBJECTS) example_tcpclient.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJECTS) $(TARGET) $(TEST_TARGET1) $(TEST_TARGET2)
