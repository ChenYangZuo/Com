//
// Created by 左晨洋 on 2025/12/27.
//

#ifndef COM_TIMEQUEUE_H
#define COM_TIMEQUEUE_H

#include <memory>

#include "TimerId.h"
#include "Timer.h"
#include "Channel.h"

namespace Com {

class EventLoop;

class TimerQueue {
public:
    explicit TimerQueue(std::shared_ptr<EventLoop> loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

    void removeTimer(TimerId id);

private:
    std::weak_ptr<EventLoop> m_loop;
    Channel m_channel;
};

} // Com

#endif //COM_TIMEQUEUE_H
