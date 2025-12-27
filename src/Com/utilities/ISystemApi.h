//
// Created by 左晨洋 on 2025/12/20.
//

#ifndef COM_ISYSTEMAPI_H
#define COM_ISYSTEMAPI_H

#include <thread>

class ISystemApi {
public:
    ISystemApi() = default;
    virtual ~ISystemApi() = default;

    virtual std::thread::id getThreadId() const = 0;
};

#endif //COM_ISYSTEMAPI_H
