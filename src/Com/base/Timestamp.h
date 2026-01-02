//
// Created by zuo on 2026/1/2.
//

#ifndef COM_TIMESTAMP_H
#define COM_TIMESTAMP_H

#include <chrono>
#include <string>

namespace Com {
class Timestamp {
public:
    // 定义时钟类型与时间点类型（微秒精度）
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock, std::chrono::microseconds>;

    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpochArg);
    explicit Timestamp(TimePoint tp);

    // 运算符重载
    bool operator==(const Timestamp &rhs) const;
    bool operator<(const Timestamp &rhs) const;

    void swap(Timestamp &that) noexcept;

    // 基础信息获取
    std::string toString() const;

    bool valid() const;

    // 内部转换接口
    int64_t microSecondsSinceEpoch() const;
    time_t secondsSinceEpoch() const;

    static Timestamp now();
    static Timestamp invalid();
    static Timestamp fromUnixTime(time_t t, int microseconds = 0);

    static constexpr int kMicroSecondsPerSecond = 1000 * 1000;

private:
    TimePoint m_timepoint;
};

// 辅助函数
inline double timeDifference(Timestamp high, Timestamp low) {
    auto diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    auto delta = std::chrono::microseconds(static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond));
    // 利用 chrono 自带的运算符处理 time_point 的加法
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta.count());
}

} // Com

#endif //COM_TIMESTAMP_H
