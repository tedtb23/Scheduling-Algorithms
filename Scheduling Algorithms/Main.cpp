#pragma once
#include "WindowProc.h"
#include "Render.h"
#include "SchedSim.h"
#include "SchedStats.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <memory>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int nCmdShow) {
    SimInfo simInfo = simulate(100);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"SchedSimWindowClass";

    WNDCLASSEX wc = { };

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return GetLastError();
    }

    // Create the window.
    HWND hwnd;

    if (!(hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Scheduling Simulation",       // Window text
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_VSCROLL,  // Window style
        
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 817,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        &simInfo    // Pass simulation data to window
    )))
    {
        MessageBox(NULL, L"Failed to create window.", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return GetLastError();
    }

    ShowWindow(hwnd, nCmdShow);

    //Get monitor dimensions
    //if (!GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo)) {
       // MessageBox(NULL, L"Failed to get monitor info.", L"Error", MB_ICONEXCLAMATION | MB_OK);
       // return ERROR_MONITOR_NO_DESCRIPTOR;
    //}

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}