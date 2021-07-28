//
// Created by 蒋澳然 on 2021/7/28.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_HTTPCODE_MAP_H
#define EPOLLLEARN_HTTPCODE_MAP_H

#include "mutex"
#include "map"

class httpcode_map {
public:
    static httpcode_map *instance() {
        if (instance_ == nullptr) {
            {
                std::unique_lock<std::mutex> lck(mutex_);
                if (instance_ == nullptr) {
                    instance_ = new httpcode_map;
                }
                lck.unlock();
            }
        }
        return instance_;
    }

    std::map<int, std::string> &get_code_map() {
        return code_map_;
    }

    httpcode_map(const httpcode_map &other) = delete;

    httpcode_map &operator=(const httpcode_map &other) = delete;

private:
    std::map<int, std::string> code_map_{};

protected:
    httpcode_map();

private:
    static std::mutex mutex_;
    static httpcode_map *instance_;
};

#define http_codes httpcode_map::instance()->get_code_map()

#endif //EPOLLLEARN_HTTPCODE_MAP_H
