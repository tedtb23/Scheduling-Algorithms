#include <Windows.h>
#include "SimInfo.hpp"
#include "SchedSim.hpp"
#include "WindowProc.hpp"
#include "Render.hpp"
#include "TrackBar.hpp"
#include <CommCtrl.h>

SimInfo* GetSimInfo(HWND hwnd) {
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    SimInfo* simInfoPtr = reinterpret_cast<SimInfo*>(ptr);
    return simInfoPtr;
}

inline void setScrollBoundsAndPage(const HWND& hwnd, SCROLLINFO& si, int trackbarPos, int scrollAmount) {
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = trackbarPos - 2;
    RECT r;
    GetClientRect(hwnd, &r);
    si.nPage = (r.bottom - r.top) / scrollAmount;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SimInfo* simInfoPtr;
    LRESULT result = 0;

    //for scrolling
    constexpr int scrollAmount = 130; //scroll amount
    static int yPos; // current vertical scrolling position 
    SCROLLINFO si;
    ///

    //for trackbar
    HWND hwndControl;
    static HWND hwndTrackbar;
    static int minProcesses;
    static int maxProcesses;
    constexpr LONG trackbarX = 350;
    constexpr LONG trackbarY = 0;
    constexpr UINT trackbarWidth = 400;
    constexpr UINT trackbarHeight = 40;
    constexpr RECT trackbarInvalidRegion = { trackbarX, trackbarY + trackbarHeight, trackbarX + trackbarWidth, trackbarY + trackbarHeight + 100};
    constexpr COLORREF bkTrkRGB = RGB(30, 30, 30);
    static HBRUSH hbrBkgnd = NULL;
    static int trackbarPos;
    ///

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        simInfoPtr = reinterpret_cast<SimInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)simInfoPtr);

        minProcesses = simInfoPtr->minProcesses;
        maxProcesses = simInfoPtr->maxProcesses;
        trackbarPos = 10;

        hwndTrackbar = createTrackBar(hwnd, NULL,
            trackbarX, //x
            trackbarY, //y
            trackbarWidth, //width
            trackbarHeight, //height
            minProcesses, //min value in range
            maxProcesses, //max value in range
            minProcesses, //min value in selection
            maxProcesses); //max value in selection

        setScrollBoundsAndPage(hwnd, si, trackbarPos, scrollAmount);

        return 0;
    }
    else {
        simInfoPtr = GetSimInfo(hwnd);
    }

    switch (uMsg) {
    case WM_DESTROY: {
        DeleteObject(hbrBkgnd);
        PostQuitMessage(0);
        break;
    }
    case WM_PAINT: {
        UpdateSimStats(hwnd, *simInfoPtr);
        break;
    }
    case WM_CHAR: {
        if (wParam == VK_RETURN) {
            *simInfoPtr = simulate(trackbarPos);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)simInfoPtr);
            InvalidateRect(hwnd, NULL, true);
            UpdateWindow(hwnd);
            setScrollBoundsAndPage(hwnd, si, trackbarPos, scrollAmount);
        }
        if (wParam == VK_ESCAPE) SendMessage(hwnd, WM_DESTROY, NULL, NULL);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);

        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(bkTrkRGB);
        }
        result = (INT_PTR)hbrBkgnd;
        break;
    }
    case WM_HSCROLL:
    {
        hwndControl = (HWND)lParam;
        
        if (hwndTrackbar == hwndControl) //if control responsible for hscroll is the trackbar.
        {
            SetFocus(hwnd); //prevent trackbar from keeping focus
            switch (LOWORD(wParam)) {
                
            case TB_ENDTRACK:

                trackbarPos = (int) SendMessage(hwndTrackbar, TBM_GETPOS, 0, 0);

                if (trackbarPos > maxProcesses)
                    SendMessage(hwndTrackbar, TBM_SETPOS,
                        (WPARAM)TRUE,       // redraw flag 
                        (LPARAM)maxProcesses);

                else if (trackbarPos < minProcesses)
                    SendMessage(hwndTrackbar, TBM_SETPOS,
                        (WPARAM)TRUE,       // redraw flag 
                        (LPARAM)minProcesses);
                break;

            default:

                break;
            }
            //invalidate the region below the trackbar 
            //which contains the display for the trackbar position.
            InvalidateRect(hwnd, &trackbarInvalidRegion, true); 
            UpdateWindow(hwnd);
        }
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
            ScrollWindow(hwnd, 0, scrollAmount * (yPos - si.nPos), NULL, NULL);
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
            ScrollWindow(hwnd, 0, scrollAmount * (yPos - si.nPos), NULL, NULL);
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