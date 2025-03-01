#include "ScreenshotHandler.h"


ScreenshotHandler::ScreenshotHandler() = default;

ScreenshotHandler::~ScreenshotHandler() {
    stop();
}

ScreenshotHandler &ScreenshotHandler::getInstance() {
    static ScreenshotHandler instance;
    return instance;
}

void ScreenshotHandler::start() {
    running = true;
    workerThread = std::thread(&ScreenshotHandler::HandleScreenshots, this);
}

void ScreenshotHandler::stop() {
    running = false;
    queueCondition.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void ScreenshotHandler::HandleScreenshots() {
}

int ScreenshotHandler::TakeScreenshot(HWND hWnd) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
    if (!hdcMemDC) {
        printf("%s\n", "CreateCompatibleDC failed");
        return -1;
    }

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
    if (!hbmScreen) {
        printf("%s\n", "CreateCompatibleBitmap failed.");
        DeleteDC(hdcMemDC);
        ReleaseDC(NULL, hdcScreen);
        return -1;
    }

    SelectObject(hdcMemDC, hbmScreen);
    if (!BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY)) {
        printf("%s\n", "BitBlt failed.");
        DeleteObject(hbmScreen);
        DeleteDC(hdcMemDC);
        ReleaseDC(NULL, hdcScreen);
        return -1;
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Bitmap bitmap(hbmScreen, NULL);

    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    bitmap.Save(L"screenshot.png", &pngClsid, NULL);

    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);

    return 0;
}

// Function to get the CLSID of the image encoder for a specific format
int ScreenshotHandler::GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    auto *pImageCodecInfo = static_cast<ImageCodecInfo *>(malloc(size));
    if (!pImageCodecInfo)
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;
        }
    }
    free(pImageCodecInfo);
    return -1;
}
