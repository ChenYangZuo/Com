//
// Created by 左晨洋 on 2025/12/27.
//

#include "TimerQueue.h"

#include <memory>
#include <string>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <sys/timerfd.h>

#include "EventLoop.h"
#include "Channel.h"
#include "timer/Timer.h"
#include "timer/TimerId.h"

namespace Com {
namespace {
int createTimerfd() {
    const int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd < 0) {
        const std::string msg = "Failed to create timerfd";
        spdlog::error(msg);
        throw std::runtime_error(msg);
    }
    return fd;
}

void readTimerfd(const int timerfd, Timestamp now) {
    uint64_t howmany;
    if (ssize_t n = ::read(timerfd, &howmany, sizeof howmany); n != sizeof howmany) {
        spdlog::error("readTimerfd() reads {} bytes instead of 8", n);
    }
}

void resetTimerfd(const int timerfd, Timestamp expiration) {
    itimerspec newValue = {};
    int64_t microSeconds = expiration.microSecondsSinceEpoch();
    timespec ts{};
    ts.tv_sec = microSeconds / Timestamp::kMicroSecondsPerSecond;
    ts.tv_nsec = microSeconds % Timestamp::kMicroSecondsPerSecond * 1000;
    newValue.it_value = ts;
    ::timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &newValue, nullptr);
}
}

TimerQueue::TimerQueue(EventLoop *loop)
    : m_loop(loop),
      m_timerfd(createTimerfd()),
      m_channel(std::make_shared<Channel>(loop, m_timerfd)) {
}

TimerQueue::~TimerQueue() {
    m_channel->disableAll();
    m_channel->remove();
    ::close(m_timerfd);
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    auto timer = std::make_shared<Timer>(std::move(cb), when, interval);
    m_loop->runInLoop([this, timer] { addTimerInLoop(timer); });
    return {timer.get(), timer->sequence()};
}

void TimerQueue::addTimerInLoop(const std::shared_ptr<Timer> &timer) {
    m_loop->assertInLoopThread();
    if (insert(timer)) {
        resetTimerfd(m_timerfd, timer->expiration());
    }
}

bool TimerQueue::insert(const std::shared_ptr<Timer> &timer) {
    m_loop->assertInLoopThread();
    bool earliestChanged = false;
    auto when = timer->expiration();
    if (const auto it = m_timers.begin(); it == m_timers.end() || when < it->first) {
        earliestChanged = true;
    }
    m_timers.insert(Entry(when, timer));
    m_activeTimers.insert(ActiveTimer(timer, timer->sequence()));
    return earliestChanged;
}

void TimerQueue::removeTimer(TimerId id) {
    m_loop->runInLoop([this, id] { removeTimerInLoop(id); });
}

void TimerQueue::initialize() {
    m_channel->setReadCallback([this] { handleRead(); });
    m_channel->enableRead();
}

void TimerQueue::removeTimerInLoop(TimerId timerId) {
    m_loop->assertInLoopThread();
    ActiveTimer timer(timerId.m_timer, timerId.m_sequence);
    auto it = m_activeTimers.find(timer);
    if (it != m_activeTimers.end()) {
        m_timers.erase(Entry(it->first->expiration(), it->first));
        m_activeTimers.erase(it);
    } else if (m_callingExpiredTimers) {
        // 该定时器正在执行
        m_cancelingTimers.insert(timer);
    }
}

void TimerQueue::handleRead() {
    m_loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);

    m_callingExpiredTimers = true;
    m_cancelingTimers.clear();
    for (const Entry &it: expired) {
        it.second->run();
    }
    m_callingExpiredTimers = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry(now, nullptr);
    auto end = m_timers.lower_bound(sentry);
    std::copy(m_timers.begin(), end, back_inserter(expired));
    m_timers.erase(m_timers.begin(), end);
    for (const Entry &it: expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        m_activeTimers.erase(timer);
    }
    return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {
    Timestamp nextExpire;
    for (const Entry &it: expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat() && m_cancelingTimers.find(timer) == m_cancelingTimers.end()) {
            it.second->restart(now);
            insert(it.second);
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        resetTimerfd(m_timerfd, nextExpire);
    }
}
} // Com
