//
// Created by akerm on 26/02/2025.
//

#include "MouseHandler.h"

std::unordered_map<int, ClickInfo> clickMap;

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
