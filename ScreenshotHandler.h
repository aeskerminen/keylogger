#ifndef SCREENSHOTHANDLER_H
#define SCREENSHOTHANDLER_H

#include <condition_variable>

#include "Windows.h"
#include <gdiplus.h>
#include <stdio.h>
#include <thread>


using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "rpcrt4.lib")

class ScreenshotHandler {
public:
    static ScreenshotHandler &getInstance();

    void HandleScreenshots();

    int TakeScreenshot(HWND hWnd);

    void start();

    void stop();

private:
    ScreenshotHandler();

    ~ScreenshotHandler();


    std::thread workerThread;

    bool running = false;

    std::string lastTitle = "";
};

#endif //SCREENSHOTHANDLER_H
