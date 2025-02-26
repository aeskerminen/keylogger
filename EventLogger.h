#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <atomic>


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

    std::queue<std::string> eventQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::thread workerThread;
    std::atomic<bool> running;
};

#endif
