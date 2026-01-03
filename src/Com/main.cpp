#include "EventLoop.h"

#include "spdlog/spdlog.h"
#include <functional>


int main() {
    // spdlog::set_level(spdlog::level::trace);

    auto eventLoop2 = Com::EventLoop::create();

    eventLoop2->runAfter(1, []() {
        spdlog::info("Hello, World!");
    });

    eventLoop2->runEvery(2, []() {
        spdlog::info("Hello, World!2");
    });

    eventLoop2->loop();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
