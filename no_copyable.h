//
// Created by 蒋澳然 on 2021/7/26.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_NOCOPYABLE_H
#define EPOLLLEARN_NOCOPYABLE_H

class no_copyable {
public:
    no_copyable(const no_copyable &other) = delete;

    no_copyable &operator=(const no_copyable &other) = delete;

protected:
    no_copyable() = default;

    virtual ~no_copyable() = default;
};

#endif //EPOLLLEARN_NOCOPYABLE_H
