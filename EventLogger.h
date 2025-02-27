#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <atomic>
#include <cpr/cpr.h>
#include <iostream>
#include <sstream>

struct InputEvent {
    std::string event;
    std::string timestamp;
} typedef InputEvent;

class EventLogger {
public:
    static EventLogger& getInstance();
    void logEvent(const std::string& event);
    void start();
    void stop();

private:
    EventLogger();
    ~EventLogger();
    void processQueue();

    std::queue<InputEvent> eventQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::thread workerThread;
    std::atomic<bool> running;
};

#endif
