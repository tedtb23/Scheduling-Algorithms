#pragma once
#include <Windows.h>
#include "SimInfo.h"
#include "WindowProc.h"
#include "Render.h"

SimInfo* GetSimInfo(HWND hwnd) {
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    SimInfo* simInfoPtr = reinterpret_cast<SimInfo*>(ptr);
    return simInfoPtr;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static const int prosTextLength = 130;
    SimInfo* simInfoPtr;
    LRESULT result = 0;
    SCROLLINFO si;
    RECT windowRect;
    HDC ctx;
    static int yPos; // current vertical scrolling position 

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        simInfoPtr = reinterpret_cast<SimInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)simInfoPtr);
        return 0;
    }
    else {
        simInfoPtr = GetSimInfo(hwnd);
    }

    switch (uMsg) {
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_PAINT: {
        UpdateSimStats(hwnd, *simInfoPtr);
        break;
    }
    case WM_CHAR: {
        if (wParam == VK_RETURN) MessageBox(NULL, L"Test.", L"Test", MB_ICONEXCLAMATION | MB_OK);
        if (wParam == VK_ESCAPE) SendMessage(hwnd, WM_DESTROY, NULL, NULL);
        break;
    }
    case WM_VSCROLL: {
        // Get all the vertial scroll bar information.
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        // Save the position for comparison later on.
        yPos = si.nPos;
        switch (LOWORD(wParam))
        {

            // User clicked the HOME keyboard key.
        case SB_TOP:
            si.nPos = si.nMin;
            break;

            // User clicked the END keyboard key.
        case SB_BOTTOM:
            si.nPos = si.nMax;
            break;

            // User clicked the top arrow.
        case SB_LINEUP:
            si.nPos -= 1;
            break;

            // User clicked the bottom arrow.
        case SB_LINEDOWN:
            si.nPos += 1;
            break;

            // User clicked the scroll bar shaft above the scroll box.
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;

            // User clicked the scroll bar shaft below the scroll box.
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;

            // User dragged the scroll box.
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;

        default:
            break;
        }

        // Set the position and then retrieve it.  Due to adjustments
        // by Windows it may not be the same as the value set.
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hwnd, SB_VERT, &si);

        // If the position has changed, scroll window and update it.
        if (si.nPos != yPos)
        {
            ScrollWindow(hwnd, 0, prosTextLength * (yPos - si.nPos), NULL, NULL);
            UpdateWindow(hwnd);
        }
        break;
    }
    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        // Save the position for comparison later on.
        yPos = si.nPos;

        si.nPos += -zDelta / 120; //120 is wheel delta
        if (si.nPos < 0) si.nPos = 0;

        // Set the position and then retrieve it.  Due to adjustments
        // by Windows it may not be the same as the value set.
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hwnd, SB_VERT, &si);

        // If the position has changed, scroll window and update it.
        if (si.nPos != yPos)
        {
            ScrollWindow(hwnd, 0, prosTextLength * (yPos - si.nPos), NULL, NULL);
            UpdateWindow(hwnd);
        }
        break;
    }
    default: {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
}