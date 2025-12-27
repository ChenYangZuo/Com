#include "EventLoop.h"

int main() {
    // auto eventLoop1 = Com::EventLoop::create();
    // eventLoop1.reset();
    auto eventLoop2 = Com::EventLoop::create();
    eventLoop2->loop();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
