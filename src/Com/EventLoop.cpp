//
// Created by 左晨洋 on 2025/12/20.
//

#include "EventLoop.h"

#include <unistd.h>
#include <sys/eventfd.h>
#include <spdlog/spdlog.h>

#include <utility>

#include "Channel.h"
#include "Poller.h"
#include "timer/TimerQueue.h"
#include "utilities/SystemApi.h"
#include "base/Timestamp.h"


namespace {
    int createEventfd() {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            std::string msg = "Failed in eventfd";
            spdlog::error(msg);
            throw std::runtime_error(msg);
        }
        return evtfd;
    }
}

namespace Com {

constexpr int kPollTimeMs = 3000;

thread_local std::weak_ptr<EventLoop> t_eventLoop{};

EventLoop::EventLoop()
    : m_systemApi(std::make_unique<SystemApi>()),
      m_poller(Poller::create(this)),
      m_timerQueue(std::make_unique<TimerQueue>(this)),
      m_wakeupFd(createEventfd()),
      m_wakeupChannel(std::make_shared<Channel>(this, m_wakeupFd)){
    m_threadId = m_systemApi->getThreadId();
    m_timerQueue->initialize();
    updateChannel(m_wakeupChannel);
    spdlog::info("EventLoop constructed in thread {}", std::hash<std::thread::id>{}(m_threadId));
}

EventLoopPtr EventLoop::create() {
    auto loop = EventLoopPtr(new EventLoop());

    if (!t_eventLoop.expired()) {
        spdlog::error("Another EventLoop existed in this thread");
        return nullptr;
    }

    t_eventLoop = loop;
    return loop;
}

EventLoop::~EventLoop() {
    t_eventLoop.reset();
}

void EventLoop::loop() {
    assert(!m_isRunning);
    assertInLoopThread();
    m_isRunning = true;
    m_quit = false;
    while (!m_quit) {
        m_activeChannels.clear();
        m_poller->poll(kPollTimeMs, m_activeChannels);
        for (const auto& it : m_activeChannels) {
            it.lock()->handleEvent();
        }
        execFunctors();
    }

    spdlog::info("EventLoop loop exited");
    m_isRunning = false;
}

void EventLoop::quit() {
    m_quit = true;
}

void EventLoop::updateChannel(const std::shared_ptr<Channel> &channel) const {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(const std::shared_ptr<Channel> &channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->removeChannel(channel);
}

void EventLoop::assertInLoopThread() const {
    if (!isInLoopThread()) {
        std::string msg = fmt::format(
            "EventLoop was created in thread {}, current thread is {}",
            std::hash<std::thread::id>{}(m_threadId),
            std::hash<std::thread::id>{}(m_systemApi->getThreadId())
        );
        spdlog::error(msg);
        throw std::runtime_error(msg);
    }
}

bool EventLoop::isInLoopThread() const {
    return m_threadId == m_systemApi->getThreadId();
}

void EventLoop::runInLoop(Functor callback) {
    if (isInLoopThread()) {
        callback();
        // 如果是在EventLoop的线程中调用的，那就直接执行
    } else {
        queueInLoop(std::move(callback));
        // 否则将函数压入队列等待执行
    }
}

void EventLoop::queueInLoop(Functor callback) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_functors.push_back(std::move(callback));
    }
    if (!isInLoopThread() || m_callingFunctor) {
        wakeup();
    }
    /* 如果 IO 线程正在执行 Functors，此时它并不阻塞在 epoll_wait
     * 但如果执行任务的过程中，又有任务通过 queueInLoop 进来了
     * 如果没有这个标志位，程序可能不会执行 wakeup
     * 当 doPendingFunctors 执行完，回到循环头部进入 epoll_wait 时，新任务就会被“卡住”，直到下一次有网络事件发生才能被执行
     * 加上这个标志位，能确保即使在任务处理中，也能通过 eventfd 给内核留一个“闹钟”，让下一次 epoll_wait 立即返回
     */
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback callback) {
    return m_timerQueue->addTimer(std::move(callback), time, 0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback callback) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(callback));
}

TimerId EventLoop::runEvery(double interval, TimerCallback callback) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return m_timerQueue->addTimer(std::move(callback), time, interval);
}

void EventLoop::cancel(TimerId timerId) {
    m_timerQueue->removeTimer(timerId);
}

void EventLoop::wakeup() const {
    constexpr uint64_t one = 1;
    // TODO: 封装write接口
    if (ssize_t n = ::write(m_wakeupFd, &one, sizeof(one)); n != sizeof(one)) {
        spdlog::error("EventLoop::wakeup() writes {} bytes instead of 8", n);
    }
}

void EventLoop::execFunctors() {
    std::vector<Functor> functors;
    m_callingFunctor = true;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        functors.swap(m_functors);
    }
    for (auto &functor : functors) {
        functor();
    }
    m_callingFunctor = false;
}
} // Com
