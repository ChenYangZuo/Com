//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_EPOLLPOLLER_H
#define COM_EPOLLPOLLER_H

#include "Poller.h"

namespace Com {
class EPollPoller final : public Poller{
public:
    explicit EPollPoller(std::shared_ptr<EventLoop> loop);
    ~EPollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList &activeChannels) override;

    void updateChannel(std::weak_ptr<Channel> channel) override;

    void removeChannel(std::weak_ptr<Channel> channel) override;
};
} // Com

#endif //COM_EPOLLPOLLER_H
