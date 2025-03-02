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
    if (workerThread.joinable()) {
        workerThread.join();
    }
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

    const CLSID pngClsid = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };

    UUID uuid;
    RPC_CSTR uuidStr;
    UuidToStringA(&uuid, &uuidStr);
    // Convert ASCII to wide string
    std::string temp(reinterpret_cast<char*>(uuidStr));
    std::wstring filename(temp.begin(), temp.end());
    filename += L".png";
    RpcStringFreeA(&uuidStr);

    auto* pBitmap = new Bitmap(hbmScreen, nullptr);
    if (pBitmap->GetLastStatus() != Ok) {
        delete pBitmap;
        DeleteObject(hbmScreen);
        DeleteDC(hdcMemDC);
        ReleaseDC(NULL, hdcScreen);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    pBitmap->Save(filename.c_str(), &pngClsid, nullptr);
    delete pBitmap;

    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(nullptr, hdcScreen);
    GdiplusShutdown(gdiplusToken);

    return 0;
}

void ScreenshotHandler::HandleScreenshots() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (const HWND hWnd = GetForegroundWindow()) {
            const size_t n = 256;
            char buffer[n];
            GetWindowTextA(hWnd, buffer, n);

            if (lastTitle != std::string(buffer)) {
                printf("%s\n", "Focus changed.");
                TakeScreenshot(nullptr);
            }

            lastTitle = std::string(buffer);
        }
    }
}

