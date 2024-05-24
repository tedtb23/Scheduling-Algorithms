#include "WindowProc.hpp"
#include "Render.hpp"
#include "SchedSim.hpp"
#include "SchedStats.hpp"
#include <Windows.h>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int nCmdShow) {
    SimInfo simInfo;
    try {
        simInfo = simulate(10, 2);
    }
    catch (std::invalid_argument invalidArg) {
        MessageBoxA(NULL, invalidArg.what(), "Error", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

   int height = GetSystemMetrics(SM_CYSCREEN);

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
        
        // Position and size
        0, 0, 900, height - 90,

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
    

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}