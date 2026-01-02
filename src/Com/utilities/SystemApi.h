//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_SYSTEMAPI_H
#define COM_SYSTEMAPI_H

#include "ISystemApi.h"

namespace Com {

class SystemApi final : public ISystemApi {
public:
    SystemApi() = default;
    ~SystemApi() override = default;

    std::thread::id getThreadId() const override;
};

}

#endif //COM_SYSTEMAPI_H
