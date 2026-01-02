//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_EVENTLOOP_H
#define COM_EVENTLOOP_H

#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <mutex>
#include <functional>

#include "base/Noncopyable.h"
#include "utilities/ISystemApi.h"
#include "timer/TimerId.h"
#include "timer/Timer.h"

namespace Com {

class Channel;
class Poller;
class EventLoop;
class TimerQueue;

using EventLoopPtr = std::shared_ptr<EventLoop>;
using ChannelList = std::vector<std::weak_ptr<Channel>>;
using Functor = std::function<void()>;

class EventLoop : public Noncopyable{
public:
    static EventLoopPtr create();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(const std::shared_ptr<Channel>& channel) const;

    void assertInLoopThread() const;
    bool isInLoopThread() const;
    void runInLoop(Functor callback);
    void queueInLoop(Functor callback);

    TimerId runAt(Timestamp time, TimerCallback callback);
    TimerId runAfter(double delay, TimerCallback callback);
    TimerId runEvery(double interval, TimerCallback callback);
    void cancel(TimerId timerId);
    void wakeup() const;

private:
    EventLoop();
    void execFunctors();

    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_quit{false};
    std::thread::id m_threadId;
    std::unique_ptr<ISystemApi> m_systemApi;
    std::unique_ptr<Poller> m_poller;
    std::unique_ptr<TimerQueue> m_timerQueue;
    ChannelList m_activeChannels;

    std::mutex m_mutex;
    std::vector<Functor> m_functors;
    std::atomic<bool> m_callingFunctor{false};
    std::unique_ptr<Channel> m_wakeupChannel;
    int m_wakeupFd;
};

} // Com

#endif //COM_EVENTLOOP_H
