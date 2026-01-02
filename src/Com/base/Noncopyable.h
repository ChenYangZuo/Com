//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_NONCOPYABLE_H
#define COM_NONCOPYABLE_H

namespace Com {

class Noncopyable {
protected:
    constexpr Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

}

#endif //COM_NONCOPYABLE_H
