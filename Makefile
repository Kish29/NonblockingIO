# 1、wildcard : 扩展通配符
# 2、notdir ： 去除路径
# 3、patsubst ：替换通配符
MAIN_SOURCE := main.cpp
# 获取所有.cpp文件
SOURCE		:= $(wildcard *.cpp tests/*.cpp)
# 过滤掉Main.o
# (filter pattern, TEXT) 保留text中符合pattern的字符串
# (filter-out pattern, TEXT) 去掉text中符合pattern的字符串
override SOURCE := $(filter-out $(MAIN_SOURCE), $(SOURCE))
# 将所有的.cpp编译成.o文件
OBJECTS 	:= $(patsubst %.cpp, %.o, $(SOURCE))

TARGET		:= epoll_learn
#TEST_TARGET	:= Test1
CXX			:= g++
LIBS		:= -lpthread
CFLAGS		:= -std=c++11 -g -Wall -O3
CXXFLAGS	:= $(CFLAGS)

.PHONY	: all clean

all	: $(TARGET)


$(TARGET) : $(OBJECTS) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)


clean:
	rm -rf *.o $(TARGET)
