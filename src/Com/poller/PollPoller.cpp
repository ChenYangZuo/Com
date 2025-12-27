//
// Created by 左晨洋 on 2025/12/20.
//

#include "PollPoller.h"

#include <string>
#include <poll.h>
#include <spdlog/spdlog.h>

#include "Channel.h"

namespace Com {

PollPoller::PollPoller(EventLoop *loop)
    : Poller(loop) {
}

PollPoller::~PollPoller() = default;

/// @brief 调用poll获取活动IO事件，填充调用方activeChannels，返回poll的返回时刻
/// @param timeoutMs
/// @param activeChannels
/// @return
Timestamp PollPoller::poll(const int timeoutMs, ChannelList &activeChannels) {
    int numEvents = ::poll(m_pollFds.data(), m_pollFds.size(), timeoutMs);
    const Timestamp now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    if (numEvents > 0) {
        spdlog::trace("{} events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        spdlog::trace("no events happened");
    } else {
        const std::string msg = "PollPoller::poll()";
        spdlog::error(msg);
        throw std::runtime_error(msg);
    }
    return now;
}

/// @brief 维护与更新m_pollfds，index用于记住Channel在m_pollfds的下标，加快更新的速度
/// @param wChannel
void PollPoller::updateChannel(std::weak_ptr<Channel> wChannel) {
    assertInLoopThread();
    auto channel = wChannel.lock();
    if (channel->index() < 0) {
        assert(m_channels.find(channel->fd()) == m_channels.end());
        pollfd pfd = {channel->fd(), static_cast<short>(channel->events()), 0};
        m_pollFds.push_back(pfd);
        int index = static_cast<int>(m_pollFds.size()) - 1;
        channel->setIndex(index);
        m_channels[pfd.fd] = wChannel;
    } else {
        assert(m_channels.find(channel->fd()) == m_channels.end());
        assert(m_channels[channel->fd()].lock() == channel);
        int index = channel->index();
        assert(index >= 0 && index < m_pollFds.size());
        pollfd &pfd = m_pollFds[index];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            // channel不关心任何事件
            pfd.fd = -1;
        }
    }
}

void PollPoller::removeChannel(std::weak_ptr<Channel> channel) {
}

/// @brief 遍历关注的fd，找出活动fd，将对应Channel填入activeChannels
/// @attention 不能一边遍历一边调用Channel::handleEvent()
/// @param numEvents
/// @param activeChannels
void PollPoller::fillActiveChannels(int numEvents, ChannelList &activeChannels) const {
    for (auto pfd = m_pollFds.begin(); pfd != m_pollFds.end() && numEvents > 0; ++pfd) {
        // revents: 监控事件中满足条件返回的事件
        if (pfd->revents > 0) {
            --numEvents;
            auto ch = m_channels.find(pfd->fd);
            assert(ch != m_channels.end());
            auto channel = ch->second.lock();
            assert(channel->fd() == pfd->fd);
            channel->setRevents(pfd->revents);
            activeChannels.push_back(channel);
        }
    }
}
} // Com
