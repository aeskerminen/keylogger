#ifndef SCREENSHOTHANDLER_H
#define SCREENSHOTHANDLER_H

#include "Windows.h"


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
int TakeScreenshot(HWND hWnd);

#endif //SCREENSHOTHANDLER_H
