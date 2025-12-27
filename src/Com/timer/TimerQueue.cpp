//
// Created by 左晨洋 on 2025/12/27.
//

#include "TimerQueue.h"

#include <sys/timerfd.h>

namespace Com {

int createTimerfd() {
    int fd = ::timerfd_create();
}

TimerQueue::TimerQueue(std::shared_ptr<EventLoop> loop) : m_channel() {
}

TimerQueue::~TimerQueue() {
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
}

void TimerQueue::removeTimer(TimerId id) {
}
} // Com
