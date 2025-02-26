//
// Created by akerm on 26/02/2025.
//

#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <windows.h>
#include <unordered_map>
#include <Psapi.h>

struct ClickInfo {
    DWORD64 clickTime;
    POINT pos;
    int type;
} typedef ClickInfo;

const std::unordered_map<int, const char *> MOUSE_EVENT_TO_STRING{
        {VK_LBUTTON, "Left Click"}, {VK_RBUTTON, "Right Click"}
};


constexpr int doubleClickThresholdMS = 300;
constexpr int doubleClickDistThreshold = 5;

LRESULT CALLBACK MouseCallback(const int nCode, const WPARAM wParam, const WPARAM lParam);

#endif //MOUSEHANDLER_H
