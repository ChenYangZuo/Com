//
// Created by 左晨洋 on 2025/12/27.
//

#ifndef COM_TIMER_H
#define COM_TIMER_H

#include <functional>
#include <atomic>
#include <utility>

#include "base/Noncopyable.h"
#include "base/Timestamp.h"

namespace Com {

using TimerCallback = std::function<void()>;

class Timer : public Noncopyable {
public:
    Timer(TimerCallback callback, Timestamp when, double interval)
        : m_callback(std::move(callback)),
          m_expiration(when),
          m_interval(interval),
          m_repeat(interval > 0.0),
          m_sequence(s_numCreated.fetch_add(1)) {
    }

    void run() const {
        m_callback();
    }

    Timestamp expiration() const {
        return m_expiration;
    }

    bool repeat() const {
        return m_repeat;
    }

    int64_t sequence() const {
        return m_sequence;
    }

    void restart (Timestamp now);

    static int64_t numCreated() {
        return s_numCreated.load();
    }

private:
    const TimerCallback m_callback;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
    const int64_t m_sequence;

    static std::atomic<int64_t> s_numCreated;
};

} // Com

#endif //COM_TIMER_H
