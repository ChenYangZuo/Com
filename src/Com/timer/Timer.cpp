//
// Created by 左晨洋 on 2025/12/27.
//

#include "Timer.h"

namespace Com {

void Timer::restart(Timestamp now) {
    if (m_repeat) {
        m_expiration = addTime(now, m_interval);
    } else {
        m_expiration = Timestamp::min();
    }
}

} // Com
