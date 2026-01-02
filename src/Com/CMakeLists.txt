set(TARGET_EXEC_NAME ${CMAKE_PROJECT_NAME})

add_executable(${TARGET_EXEC_NAME})

target_sources(${TARGET_EXEC_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/Com/utilities/SystemApi.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/poller/PollPoller.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/poller/EPollPoller.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/poller/PollerFactory.cpp

    ${PROJECT_SOURCE_DIR}/src/Com/base/Timestamp.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/timer/Timer.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/timer/TimerQueue.cpp

    ${PROJECT_SOURCE_DIR}/src/Com/EventLoop.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/Channel.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/Poller.cpp
    ${PROJECT_SOURCE_DIR}/src/Com/main.cpp
)

target_include_directories(${TARGET_EXEC_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/Com/
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${TARGET_EXEC_NAME} PRIVATE spdlog::spdlog)
