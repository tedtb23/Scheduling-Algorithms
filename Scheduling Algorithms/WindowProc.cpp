#include <Windows.h>
#include "SimInfo.hpp"
#include "SchedSim.hpp"
#include "WindowProc.hpp"
#include "Render.hpp"
#include "TrackBar.hpp"
#include <CommCtrl.h>
#include <stdexcept>

static inline SimInfo* GetSimInfo(HWND hwnd) {
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    return reinterpret_cast<SimInfo*>(ptr);
}

static inline void setScrollBoundsAndPage(const HWND& hwnd, SCROLLINFO& si, int trackbarPos, int maxGanttNode, int scrollAmount) {
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    //maximum scroll depth should go to the last process or gantt node, whichever is greater.
    //si.nMax = max(trackbarPos - 2, (maxGanttNode / 10)); //maybe 12 works???
    //si.nMax = trackbarPos - 2;
    si.nMax = 1000;
    RECT r;
    GetClientRect(hwnd, &r);
    si.nPage = (r.bottom - r.top) / scrollAmount;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
}

static inline void handleTrackbarMove(
    const HWND& hwnd, 
    const WPARAM& wParam, 
    const HWND& hwndTrackbar,
    int pos,
    int min,
    int max,
    RECT* trackbarRectPtr,
    RECT* trackbarInvalidRectPtr) {

    SetFocus(hwnd); //prevent trackbar from keeping focus
    switch (LOWORD(wParam)) {

    case TB_ENDTRACK:

        pos = (int)SendMessage(hwndTrackbar, TBM_GETPOS, 0, 0);

        if (pos > max)
            SendMessage(hwndTrackbar, TBM_SETPOS,
                (WPARAM)TRUE,       // redraw flag 
                (LPARAM)max);

        else if (pos < min)
            SendMessage(hwndTrackbar, TBM_SETPOS,
                (WPARAM)TRUE,       // redraw flag 
                (LPARAM)min);
        break;

    default:
        break;
    }
    //invalidate the region below the trackbar
    //which contains the display for the trackbar position.
    GetWindowRect(hwndTrackbar, trackbarRectPtr);
    MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)trackbarRectPtr, 2);
    *trackbarInvalidRectPtr =
    { trackbarRectPtr->right - 20, //region to the bottom right of trackbar
    trackbarRectPtr->bottom,      //(where the display for trackbarPos is)
    trackbarRectPtr->right + 40,
    trackbarRectPtr->bottom + 30 };
    InvalidateRect(hwnd, trackbarInvalidRectPtr, true);
    UpdateWindow(hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SimInfo* simInfoPtr;
    LRESULT result = 0;

    //for scrolling
    constexpr int scrollAmount = 130; //scroll amount
    static int yPos; // current vertical scrolling position 
    SCROLLINFO si;
    int maxGanttNode = 0;
    ///

    //for trackbars
    HWND hwndTrckControl;
    constexpr long trackbarX = 0;
    constexpr long trackbarWidth = 200;
    constexpr long trackbarHeight = 40;
    constexpr COLORREF bkTrkRGB = RGB(30, 30, 30);
    static HBRUSH trackbarHBrush = NULL;
    ///
    //for process amount trackbar
    static HWND hwndTrackbarPros;
    static int minProcesses;
    static int maxProcesses;
    static long trackbarProsY;
    RECT trackbarProsRect;
    RECT trackbarProsInvalidRect;
    static int trackbarProsPos;
    ///
    //for RR time quantum trackbar
    static HWND hwndTrackbarTQ;
    static int minTQ;
    static int maxTQ;
    static long trackbarTQY;
    RECT trackbarTQRect;
    RECT trackbarTQInvalidRect;
    static int trackbarTQPos;
    ///

    //for go button
    HWND hwndBtnControl;
    static HWND hwndGoBtn;
    constexpr long btnWidth = 30;
    constexpr long btnHeight = 30;
    constexpr long btnX = trackbarWidth;
    static long btnY;
    static HBRUSH btnHBrush = NULL;
    ///

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        simInfoPtr = reinterpret_cast<SimInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)simInfoPtr);

        minProcesses = simInfoPtr->minProcesses;
        maxProcesses = simInfoPtr->maxProcesses;
        trackbarProsPos = minProcesses;
        trackbarProsY = (long)simInfoPtr->stats.size() * (long)scrollAmount;

        minTQ = simInfoPtr->minTQ;
        maxTQ = simInfoPtr->maxTQ;
        trackbarTQPos = 2; //2 is starting simulation value
        trackbarTQY = trackbarProsY + trackbarHeight + 30;

        btnY = trackbarTQY;

        hwndTrackbarPros = createTrackBar(
            "Processes Trackbar", //window title
            hwnd, //parent handle
            NULL, //hInstance
            trackbarX, //x
            trackbarProsY, //y
            trackbarWidth, //width
            trackbarHeight, //height
            10,             //starting value for the trackbar
            minProcesses, //min value in range
            maxProcesses, //max value in range
            minProcesses, //min value in selection
            maxProcesses); //max value in selection
        
        hwndTrackbarTQ = createTrackBar(
            "Time Quantum Trackbar", //window title
            hwnd, //parent handle
            NULL, //hInstance
            trackbarX, //x
            trackbarTQY, //y
            trackbarWidth, //width
            trackbarHeight, //height
            2,          //starting value for the trackbar
            minTQ, //min value in range
            maxTQ, //max value in range
            minTQ, //min value in selection
            maxTQ); //max value in selection


        hwndGoBtn = CreateWindow(
            L"BUTTON", //class
            L"GO", //Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            btnX,         // x position 
            btnY,         // y position 
            btnWidth,        // Button width
            btnHeight,        // Button height
            hwnd,     // Parent window
            NULL,       // No menu.
            NULL, //hinstance
            NULL);      // Pointer not needed.


        for (SchedStats stat : simInfoPtr->stats) {
            if (stat.ganttChart.size() > maxGanttNode) {
                maxGanttNode = (int)stat.ganttChart.size();
            }
        }
        setScrollBoundsAndPage(hwnd, si, trackbarProsPos, maxGanttNode, scrollAmount);

        return 0;
    }
    else {
        simInfoPtr = GetSimInfo(hwnd);
    }

    switch (uMsg) {
    case WM_DESTROY: {
        DeleteObject(btnHBrush);
        DeleteObject(trackbarHBrush);
        PostQuitMessage(0);
        break;
    }
    case WM_PAINT: {
        UpdateSimStats(hwnd, *simInfoPtr);
        break;
    }
    case WM_COMMAND: {
        hwndBtnControl = (HWND)lParam;

        if (hwndGoBtn == hwndBtnControl) {
            SetFocus(hwnd); //prevent button from keeping focus
            switch (HIWORD(wParam)) {
                case BN_CLICKED: {
                    try {
                        *simInfoPtr = simulate(trackbarProsPos, trackbarTQPos);
                        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)simInfoPtr);
                        InvalidateRect(hwnd, NULL, true);
                        UpdateWindow(hwnd);
                        for (SchedStats stat : simInfoPtr->stats) {
                            if (stat.ganttChart.size() > maxGanttNode) {
                                maxGanttNode = (int)stat.ganttChart.size();
                            }
                        }
                        setScrollBoundsAndPage(hwnd, si, trackbarProsPos, maxGanttNode, scrollAmount);
                    }
                    catch (std::invalid_argument invalidArg) {
                        MessageBoxA(NULL, invalidArg.what(), "Error", MB_ICONEXCLAMATION | MB_OK);
                        result = 1;
                    }
                    break;
                }
            }
        }
        break;
    }
    case WM_CHAR: {
        if (wParam == VK_ESCAPE) SendMessage(hwnd, WM_DESTROY, NULL, NULL);
        break;
    }
    case WM_CTLCOLORBTN: {
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);

        if (btnHBrush == NULL)
        {
            btnHBrush = CreateSolidBrush(bkTrkRGB);
        }
        result = (INT_PTR)btnHBrush;
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);

        if (trackbarHBrush == NULL)
        {
            trackbarHBrush = CreateSolidBrush(bkTrkRGB);
        }
        result = (INT_PTR)trackbarHBrush;
        break;
    }
    case WM_HSCROLL:
    {
        hwndTrckControl = (HWND)lParam;

        if (hwndTrackbarPros == hwndTrckControl) //if control responsible for hscroll is the process trackbar.
        {
            handleTrackbarMove(
                hwnd,
                wParam,
                hwndTrackbarPros,
                trackbarProsPos,
                minProcesses,
                maxProcesses,
                &trackbarProsRect,
                &trackbarProsInvalidRect);
            trackbarProsPos = (int)SendMessage(hwndTrackbarPros, TBM_GETPOS, 0, 0);
        }
        else if (hwndTrackbarTQ == hwndTrckControl) {
            handleTrackbarMove(
                hwnd,
                wParam,
                hwndTrackbarTQ,
                trackbarTQPos,
                minTQ,
                maxTQ,
                &trackbarTQRect,
                &trackbarTQInvalidRect);
            trackbarTQPos = (int)SendMessage(hwndTrackbarTQ, TBM_GETPOS, 0, 0);
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
        #define DELTA_SCROLL 120;
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        // Save the position for comparison later on.
        yPos = si.nPos;

        si.nPos += -zDelta / DELTA_SCROLL;
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