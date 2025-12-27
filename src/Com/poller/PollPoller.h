//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_POLLPOLLER_H
#define COM_POLLPOLLER_H

#include "Poller.h"

#include <vector>

struct pollfd;

namespace Com {

class PollPoller final : public Poller {
public:
    explicit PollPoller(std::shared_ptr<EventLoop> loop);
    ~PollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList &activeChannels) override;

    void updateChannel(std::weak_ptr<Channel> wChannel) override;

    void removeChannel(std::weak_ptr<Channel> channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList &activeChannels) const;

    using PollFdArray = std::vector<pollfd>;
    PollFdArray m_pollFds;
};
} // Com

#endif //COM_POLLPOLLER_H
