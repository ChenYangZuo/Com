//
// Created by 左晨洋 on 2025/12/20.
//

#include "EventLoop.h"

#include <poll.h>
#include <spdlog/spdlog.h>

#include "Channel.h"
#include "poller/PollPoller.h"
#include "utilities/SystemApi.h"

namespace Com {

constexpr int kPollTimeMs = 10000;

thread_local std::weak_ptr<EventLoop> t_eventLoop{};

EventLoop::EventLoop()
    : m_systemApi(std::make_unique<SystemApi>()),
      m_poller(Poller::create(this)) {
    m_threadId = m_systemApi->getThreadId();
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
    }

    spdlog::info("EventLoop loop exited");
    m_isRunning = false;
}

void EventLoop::quit() {
    m_quit = true;
}

void EventLoop::updateChannel(const std::weak_ptr<Channel>& wChannel) const {
    const auto channel = wChannel.lock();
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
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

} // Com
