//
// Created by 左晨洋 on 2025/12/20.
//

#include "SystemApi.h"

namespace Com {

std::thread::id SystemApi::getThreadId() const {
    return std::this_thread::get_id();
}

}
