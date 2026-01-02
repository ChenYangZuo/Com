//
// Created by 左晨洋 on 2025/12/27.
//

#ifndef COM_TIMERID_H
#define COM_TIMERID_H

#include <cstdint>

namespace Com {

class Timer;

class TimerId {
public:
    TimerId()
        : m_timer(nullptr),
          m_sequence(0) {
    }

    TimerId(Timer *timer, int64_t sequence)
        : m_timer(timer),
          m_sequence(sequence) {
    }

    friend class TimerQueue;

private:
    Timer *m_timer;
    int64_t m_sequence;
};

} // Com

#endif //COM_TIMERID_H
