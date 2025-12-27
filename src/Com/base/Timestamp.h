//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_TIMESTAMP_H
#define COM_TIMESTAMP_H

#include <chrono>

namespace Com {

using Timestamp = std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>;
/* 时钟源的区别：
 * std::chrono::steady_clock 开机计时器，单调时钟
 * std::chrono::system_clock 墙上时钟，非单调时钟
 * std::chrono::high_resolution_clock 系统中精度最高的时钟
 */

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::duration<double>(seconds)
    );
    return timestamp + delta;
}

}

#endif //COM_TIMESTAMP_H
