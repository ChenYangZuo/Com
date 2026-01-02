//
// Created by 左晨洋 on 2025/12/20.
//

#include "Poller.h"
#include "PollPoller.h"
// #include "EPollPoller.h"

#include <cstdlib>

namespace Com {

std::unique_ptr<Poller> Poller::create(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL"))
    {
        return std::make_unique<PollPoller>(loop);
    }
    // else
    // {
    //     return std::make_shared<EPollPoller>(loop);
    // }
    return std::make_unique<PollPoller>(loop);
}

}

