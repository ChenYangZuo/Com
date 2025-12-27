//
// Created by 左晨洋 on 2025/12/20.
//

#include "Poller.h"

#include "Channel.h"

namespace Com {
Poller::Poller(std::shared_ptr<EventLoop> loop)
    : m_ownerLoop(loop){
}

bool Poller::hasChannel(std::weak_ptr<Channel> channel) const {
    assertInLoopThread();
    if (auto sp = channel.lock()) {
        auto it = m_channels.find(sp->fd());
        return it != m_channels.end() && it->second.lock() == channel.lock();
    }
    return false;
}

void Poller::assertInLoopThread() const {
    if (auto sp = m_ownerLoop.lock()) {
        sp->assertInLoopThread();
    }
}

} // Com
