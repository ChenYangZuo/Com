//
// Created by zuo on 2026/1/2.
//

#include "Timestamp.h"

#include <fmt/format.h>

namespace Com {

Timestamp::Timestamp()
    : m_timepoint(TimePoint::min()) {
}

Timestamp::Timestamp(int64_t microSecondsSinceEpochArg)
    : m_timepoint(std::chrono::microseconds(microSecondsSinceEpochArg)){
}

Timestamp::Timestamp(TimePoint tp)
    : m_timepoint(tp) {
}

bool Timestamp::operator==(const Timestamp &rhs) const {
    return m_timepoint == rhs.m_timepoint;
}

bool Timestamp::operator<(const Timestamp &rhs) const {
    return m_timepoint < rhs.m_timepoint;
}

Timestamp Timestamp::now() {
    return Timestamp(std::chrono::time_point_cast<std::chrono::microseconds>(Clock::now()));
}

Timestamp Timestamp::invalid() {
    return {};
}

Timestamp Timestamp::fromUnixTime(time_t t, int microseconds) {
    return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
}

void Timestamp::swap(Timestamp &that) noexcept {
    std::swap(m_timepoint, that.m_timepoint);
}

std::string Timestamp::toString() const {
    int64_t totalMicros = microSecondsSinceEpoch();
    int64_t seconds = totalMicros / kMicroSecondsPerSecond;
    int64_t micros = totalMicros % kMicroSecondsPerSecond;

    // {:d} 表示整数，{:06d} 表示宽度为6且高位补零
    return fmt::format("{}.{:06d}", seconds, micros);
}

bool Timestamp::valid() const {
    return m_timepoint != TimePoint::min();
}

int64_t Timestamp::microSecondsSinceEpoch() const {
    return m_timepoint.time_since_epoch().count();
}

time_t Timestamp::secondsSinceEpoch() const {
    return static_cast<time_t>(microSecondsSinceEpoch() / kMicroSecondsPerSecond);
}

} // Com