#ifndef SCREENSHOTHANDLER_H
#define SCREENSHOTHANDLER_H

#include <condition_variable>

#include "Windows.h"
#include <gdiplus.h>
#include <stdio.h>
#include <thread>


using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class ScreenshotHandler {
public:
    static ScreenshotHandler &getInstance();

    void start();

    void stop();

private:
    ScreenshotHandler();

    ~ScreenshotHandler();

    void HandleScreenshots();

    int TakeScreenshot(HWND hWnd);

    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);

    std::thread workerThread;

    std::mutex queueMutex;
    std::condition_variable queueCondition;

    bool running = false;
};

#endif //SCREENSHOTHANDLER_H
