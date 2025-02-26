//
// Created by akerm on 26/02/2025.
//

#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <string>
#include <Windows.h>
#include <unordered_set>
#include "layouts.h"

boolean checkModifierStatus(const int vkCode);
void addCombinationModifier(std::string *input);
LRESULT CALLBACK KeyboardCallback(const int nCode, const WPARAM wParam, const WPARAM lParam);

#endif //KEYBOARDHANDLER_H
