#pragma once
#include <Windows.h>
#include "SimInfo.h"

SimInfo* GetSimInfo(HWND hwnd);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
