//
// Created by 左晨洋 on 2025/12/27.
//

#ifndef COM_TIMEQUEUE_H
#define COM_TIMEQUEUE_H

#include <memory>
#include <set>

#include "TimerId.h"
#include "Timer.h"  // 这里也可以不导入头文件，使用前向声明，但是要把回调函数的类型定义提取出来单独放一个头文件导入
#include "Channel.h"

namespace Com {
class EventLoop;

class TimerQueue {
public:
    explicit TimerQueue(EventLoop *loop);

    ~TimerQueue();

    void initialize();

    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

    void removeTimer(TimerId id);

private:
    using Entry = std::pair<Timestamp, std::shared_ptr<Timer> >;
    using TimerList = std::set<Entry>; // std::set由红黑树实现，按pair.first排序，即按时间排序
    using ActiveTimer = std::pair<std::shared_ptr<Timer>, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>; // ActiveTimerSet按Timer地址排序，用于快速（O(log n)）删除Timer

    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);

    void reset(const std::vector<Entry> &expired, Timestamp now);

    void addTimerInLoop(const std::shared_ptr<Timer> &timer);

    bool insert(const std::shared_ptr<Timer> &timer);

    void removeTimerInLoop(TimerId timerId);

private:
    EventLoop *m_loop{};
    const int m_timerfd{};
    std::shared_ptr<Channel> m_channel{};
    TimerList m_timers{};
    ActiveTimerSet m_activeTimers;
    std::atomic<bool> m_callingExpiredTimers{false};
    ActiveTimerSet m_cancelingTimers{};
};
} // Com

#endif //COM_TIMEQUEUE_H
