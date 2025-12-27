//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_POLLER_H
#define COM_POLLER_H

#include <map>

#include "base/Timestamp.h"
#include "EventLoop.h"

namespace Com {

class Channel;

class Poller {
public:
    using ChannelList = std::vector<std::weak_ptr<Channel>>;

    static std::unique_ptr<Poller> create(EventLoop *loop);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList &activeChannels) = 0;

    virtual void updateChannel(std::weak_ptr<Channel> channel) = 0;
    virtual void removeChannel(std::weak_ptr<Channel> channel) = 0;
    virtual bool hasChannel(std::weak_ptr<Channel> channel) const;

    void assertInLoopThread() const;

protected:
    explicit Poller(EventLoop *loop);

    using ChannelMap = std::map<int, std::weak_ptr<Channel>>;
    ChannelMap m_channels;

private:
    EventLoop *m_ownerLoop;
};
} // Com

#endif //COM_POLLER_H
