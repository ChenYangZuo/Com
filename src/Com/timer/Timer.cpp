//
// Created by 左晨洋 on 2025/12/27.
//

#include "Timer.h"

namespace Com {

std::atomic<int64_t> Timer::s_numCreated{0};

void Timer::restart(Timestamp now) {
    if (m_repeat) {
        m_expiration = addTime(now, m_interval);
    } else {
        m_expiration = Timestamp::invalid();
    }
}

} // Com
