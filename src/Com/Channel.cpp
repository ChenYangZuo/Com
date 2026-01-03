//
// Created by 左晨洋 on 2025/12/20.
//

#include "Channel.h"

#include <poll.h>
#include <utility>
#include <spdlog/spdlog.h>

#include "EventLoop.h"

namespace Com {
namespace {
constexpr int kNoneEvent = 0;
constexpr int kReadEvent = POLLIN | POLLPRI;
constexpr int kWriteEvent = POLLOUT;
}


Channel::Channel(EventLoop *loop, const int id)
    : m_loop(loop),
      m_id(id) {
}

Channel::~Channel() = default;

void Channel::handleEvent() const {
    if (m_revents & POLLNVAL) {
        spdlog::warn("Channel::handleEvent POLLNVAL");
    }

    if (m_revents & (POLLERR | POLLNVAL)) {
        if (m_errorCallback) {
            m_errorCallback();
        }
    }

    if (m_revents & (POLLIN | POLLPRI | POLLHUP)) {
        if (m_readCallback) {
            m_readCallback();
        }
    }

    if (m_revents & POLLOUT) {
        if (m_writeCallback) {
            m_writeCallback();
        }
    }
}

void Channel::setReadCallback(const EventCallback &callback) {
    m_readCallback = callback;
}

void Channel::setWriteCallback(const EventCallback &callback) {
    m_writeCallback = callback;
}

void Channel::setErrorCallback(const EventCallback &callback) {
    m_errorCallback = callback;
}

void Channel::setReadCallback(EventCallback &&callback) {
    m_readCallback = std::move(callback);
}

void Channel::setWriteCallback(EventCallback &&callback) {
    m_writeCallback = std::move(callback);
}

void Channel::setErrorCallback(EventCallback &&callback) {
    m_errorCallback = std::move(callback);
}

int Channel::fd() const {
    return m_id;
}

int Channel::events() const {
    return m_events;
}

void Channel::setRevents(int revents) {
    m_revents = revents;
}

bool Channel::isNoneEvent() const {
    return m_events == kNoneEvent;
}

void Channel::enableRead() {
    m_events |= kReadEvent;
    update();
}

void Channel::enableWrite() {
    m_events |= kWriteEvent;
    update();
}

void Channel::disableWrite() {
    m_events &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    m_events = kNoneEvent;
    update();
}

int Channel::index() const {
    return m_index;
}

void Channel::setIndex(const int index) {
    m_index = index;
}

EventLoop *Channel::ownerLoop() const {
    return m_loop;
}

void Channel::remove() {
    m_addedToLoop = false;
    m_loop->removeChannel(shared_from_this());
}

void Channel::update() {
    m_addedToLoop = true;
    m_loop->updateChannel(shared_from_this());
}
} // Com
