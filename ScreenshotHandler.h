#ifndef SCREENSHOTHANDLER_H
#define SCREENSHOTHANDLER_H

#include "Windows.h"
#include <gdiplus.h>
#include <stdio.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class ScreenshotHandler {
public:
    static ScreenshotHandler& getInstance();
    int TakeScreenshot(HWND hWnd);
    void start();
    void stop();
private:
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};

#endif //SCREENSHOTHANDLER_H
