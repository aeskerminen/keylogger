#include "EventLogger.h"
#include <cpr/cpr.h>
#include <iostream>
#include <sstream>

const char *DEBUG_URL = "http://127.0.0.1:8000/log";

EventLogger::EventLogger() : running(false) {}

EventLogger::~EventLogger() {
    stop();
}

EventLogger& EventLogger::getInstance() {
    static EventLogger instance;
    return instance;
}

void EventLogger::logEvent(const std::string& event) {
    std::lock_guard lock(queueMutex);
    eventQueue.push(event);
    queueCondition.notify_one();
}

void EventLogger::start() {
    running = true;
    workerThread = std::thread(&EventLogger::processQueue, this);
}

void EventLogger::stop() {
    running = false;
    queueCondition.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void EventLogger::processQueue() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::unique_lock lock(queueMutex);

        if (eventQueue.empty()) {
            continue;
        }

        std::ostringstream batchData;
        while (!eventQueue.empty()) {
            batchData << eventQueue.front() << " ";
            eventQueue.pop();
        }

        lock.unlock();

        // Send data to the server
        printf("%s\n", batchData.str().c_str());
        cpr::Response response = cpr::Post(cpr::Url{DEBUG_URL}, cpr::Header{{"Content-Type", "application/json"}}, cpr::Body{"{\"text\": \"" + batchData.str() + "\"}"});
        std::cout << "Sent data to server: " << response.status_code << std::endl;
    }
}
