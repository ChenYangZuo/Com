//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_CHANNEL_H
#define COM_CHANNEL_H

#include <functional>
#include <memory>

#include "base/Noncopyable.h"

namespace Com {

using EventCallback = std::function<void()>;

class EventLoop;

class Channel : public Noncopyable, public std::enable_shared_from_this<Channel> {
public:
    Channel(EventLoop *loop, int id);
    ~Channel();

    void handleEvent() const;
    void setReadCallback(const EventCallback& callback);
    void setWriteCallback(const EventCallback& callback);
    void setErrorCallback(const EventCallback& callback);
    void setReadCallback(EventCallback&& callback);
    void setWriteCallback(EventCallback&& callback);
    void setErrorCallback(EventCallback&& callback);

    int fd() const;
    int events() const;
    void setRevents(int revents);
    bool isNoneEvent() const;

    void enableRead();
    void enableWrite();
    void disableWrite();
    void disableAll();

    int index() const;
    void setIndex(int index);

    EventLoop *ownerLoop() const;
    void remove();

private:
    void update();

    EventLoop *m_loop{};
    const int m_id{};
    int m_events{0};  // 关心的IO事件
    int m_revents{0};  // 目前活动的事件
    int m_index{-1};

    bool m_addedToLoop{false};

    EventCallback m_readCallback;
    EventCallback m_writeCallback;
    EventCallback m_errorCallback;
};

} // Com

#endif //COM_CHANNEL_H
