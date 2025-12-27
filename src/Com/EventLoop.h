//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_EVENTLOOP_H
#define COM_EVENTLOOP_H

#include <atomic>
#include <thread>
#include <memory>

#include "base/Noncopyable.h"
#include "utilities/ISystemApi.h"

namespace Com {

class Channel;
class Poller;
class EventLoop;

using EventLoopPtr = std::shared_ptr<EventLoop>;
using ChannelList = std::vector<std::weak_ptr<Channel>>;

class EventLoop : public Noncopyable, public std::enable_shared_from_this<EventLoop>{
public:
    static EventLoopPtr create();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(const std::weak_ptr<Channel>& wChannel) const;

    void assertInLoopThread() const;
    bool isInLoopThread() const;

private:
    EventLoop();

    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_quit{false};
    std::thread::id m_threadId;
    std::unique_ptr<ISystemApi> m_systemApi;
    std::unique_ptr<Poller> m_poller;
    ChannelList m_activeChannels;
};

} // Com

#endif //COM_EVENTLOOP_H
