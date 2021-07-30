//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_ERROR_MAP_H
#define EPOLLLEARN_ERROR_MAP_H

#include "mutex"
#include "map"
#include "string"

class error_map {
public:
    static error_map *instance() {
        if (instance_ == nullptr) {
            {
                std::unique_lock<std::mutex> lck(mutex_);
                if (instance_ == nullptr) {
                    instance_ = new error_map;
                }
                lck.unlock();
            }
        }
        return instance_;
    }

    std::map<int, std::string> &get_error_map() {
        return error_map_;
    }

    error_map(const error_map &other) = delete;

    error_map &operator=(const error_map &other) = delete;

private:

    std::map<int, std::string> error_map_{};

protected:
    error_map();

private:
    static std::mutex mutex_;
    static error_map *instance_;
};

#define errmap error_map::instance()->get_error_map()

#endif //EPOLLLEARN_ERRORMAP_H
