#pragma once
#include <Windows.h>

HWND WINAPI createTrackBar(
    const HWND& parent, //parent window handle
    const HINSTANCE& hInstance, //process handle
    LONG x, //x coordinate to place the trackbar
    LONG y, //y coordinate to place the trackbar
    UINT width, //horizontal size of the trackbar
    UINT height, //vertical size of the trackbar
    UINT iMin,     // minimum value in trackbar range 
    UINT iMax,     // maximum value in trackbar range 
    UINT iSelMin,  // minimum value in trackbar selection 
    UINT iSelMax);  // maximum value in trackbar selection