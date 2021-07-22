//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_ERRORMAP_H
#define EPOLLLEARN_ERRORMAP_H

#include "mutex"
#include "map"
#include "string"

class ErrorMap {
public:
    static ErrorMap *instance() {
        if (instance_ == nullptr) {
            std::unique_lock<std::mutex> lck(mutex_);
            if (instance_ == nullptr) {
                instance_ = new ErrorMap;
            }
            lck.unlock();
        }
        return instance_;
    }

    std::map<int, std::string> &get_error_map() {
        return error_map_;
    }

    ErrorMap(const ErrorMap &other) = delete;

    ErrorMap &operator=(const ErrorMap &other) = delete;

private:
    ErrorMap();

    static std::mutex mutex_;
    static ErrorMap *instance_;
    std::map<int, std::string> error_map_{};
};

#define error_map ErrorMap::instance()->get_error_map()

#endif //EPOLLLEARN_ERRORMAP_H
