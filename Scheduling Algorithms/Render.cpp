#pragma once
#include <Windows.h>
#include "SchedStats.h"
#include "Process.h"
#include "SimInfo.h"
#include "Render.h"

static void displaySchedStat(const HDC& ctx, const SchedStats& stat, int offset, const int offsetyAmount) {
    std::string avgWait = "Avg Wait: " + std::to_string(stat.avgWaitTime);
    std::string avgTurnAround = "Avg TurnAround: " + std::to_string(stat.avgTurnAroundTime);
    std::string avgBurst = "Avg Burst: " + std::to_string(stat.avgBurstTime);

    TextOutA(ctx, 0, offset, stat.AlgoUsed.c_str(), (int)stat.AlgoUsed.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgWait.c_str(), (int)avgWait.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgTurnAround.c_str(), (int)avgTurnAround.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgBurst.c_str(), (int)avgBurst.length());
    offset += offsetyAmount + 10;
}

static void displaySchedStats(const HDC& ctx, const std::vector<SchedStats>& stats, const int height, int offset, const int offsetyAmount) {
    for (SchedStats stat : stats) {
        if (offset >= height) break;
        displaySchedStat(ctx, stat, offset, offsetyAmount);
        offset += (offsetyAmount * 4) + 10;
    }
}

static void displayProcesses(
    const HDC& ctx,
    const std::vector<Process>& processes,
    const int height,
    const int startIdx, const int endIdx, 
    const int startX, const int startY, 
    const int offsetyAmount) {

    int currIdx = startIdx;
    int offset = startY;
    //const int textHeight = (offsetyAmount * 4) + 10;

    while (currIdx <= endIdx && currIdx < processes.size()) {
        if (offset >= height) break;
        const Process& p = processes[currIdx];
        std::string pid = "PID: " + std::to_string(currIdx);
        std::string arrivalTime = "Arrival Time: " + std::to_string(p.arrivalTime);
        std::string priority = "Priority: " + std::to_string(p.priority);
        std::string burstTime = "Burst Time: " + std::to_string(p.totalBurst);

        TextOutA(ctx, startX, offset, pid.c_str(), (int)pid.length());
        offset += offsetyAmount;
        TextOutA(ctx, startX, offset, arrivalTime.c_str(), (int)arrivalTime.length());
        offset += offsetyAmount;
        TextOutA(ctx, startX, offset, priority.c_str(), (int)priority.length());
        offset += offsetyAmount;
        TextOutA(ctx, startX, offset, burstTime.c_str(), (int)burstTime.length());
        offset += offsetyAmount + 10;
        currIdx++;
    }
}

void UpdateSimStats(const HWND& hwnd, const SimInfo& simInfo) {
    const int offsetxAmount = 230;
    const int offsetyAmount = 30;
    const int prosTextHeight = (offsetyAmount * 4) + 10;
    const int statTextHeight = prosTextHeight;
    int xPos; //xScroll position
    static int yPos; //yScroll position
    static int oldYPos;
    PAINTSTRUCT ps;
    SCROLLINFO si;
    HDC ctx = BeginPaint(hwnd, &ps);

    //get window bounds
    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) {
        MessageBox(NULL, L"Failed to get window bounds.", L"Error", MB_ICONEXCLAMATION | MB_OK);
    }
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    ///
    /*
    LONG top = ps.rcPaint.top;
    LONG left = ps.rcPaint.left;
    LONG bottom = ps.rcPaint.bottom;
    LONG right = ps.rcPaint.right;
    RECT updateRect = { left, top, right, bottom };
    static short r = 0;
    static short g = 0;
    static short b = 0;
    HBRUSH color = CreateSolidBrush(RGB(r,g,b));
    FillRect(ctx, &updateRect, color);
    r++;
    b++;
    if (r > 255) r = 0;
    if (g > 255) g = 0;
    if (b > 255) b = 0;
    */
    
    //select in new font and color
    const COLORREF textRGB = RGB(255, 255, 255);
    const COLORREF bkRGB = RGB(30, 30, 30);
    const COLORREF oldBKColor = SetBkColor(ctx, bkRGB);
    const COLORREF oldTextColor = SetTextColor(ctx, textRGB);
    HFONT hFont, hOldFont;
    hFont = CreateFontA(25, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    hOldFont = (HFONT)SelectObject(ctx, hFont);
    ///

    //get update region and scroll info
    LONG top = ps.rcPaint.top;
    LONG left = ps.rcPaint.left;
    LONG bottom = ps.rcPaint.bottom;
    LONG right = ps.rcPaint.right;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    oldYPos = yPos;
    yPos = si.nPos;
    GetScrollInfo(hwnd, SB_HORZ, &si);
    xPos = si.nPos;
    ///
    
    //paint stats in updated area
    if (yPos < simInfo.stats.size() && yPos >= 0) {
        if (oldYPos > yPos) { //scroll up, repaint stat which scrolled back into view
            displaySchedStat(ctx, simInfo.stats[yPos], 0, offsetyAmount);
        }
        else if (oldYPos < yPos) {  //scroll down (ignore)
            
        }
        else { //window occluded
            displaySchedStats(ctx, simInfo.stats, bottom, 0, offsetyAmount);
        }
    }

    if (yPos == 0) {
        std::string pTitle = "Processes Used : ";
        TextOutA(ctx, right - offsetxAmount - 180, 0, pTitle.c_str(), (int)pTitle.length());
    }
    
    //paint processes in updated area
    if (oldYPos > yPos) { //scroll up
        displayProcesses(ctx, simInfo.processes, bottom, yPos, simInfo.processes.size() - 1, width - offsetxAmount, top, offsetyAmount);
    }
    else if (oldYPos < yPos){ //scroll down
        displayProcesses(ctx, simInfo.processes, bottom, (top / prosTextHeight) + yPos + 1, simInfo.processes.size() - 1, width - offsetxAmount, top, offsetyAmount);
    }
    else { //window occluded
        displayProcesses(ctx, simInfo.processes, height, yPos, simInfo.processes.size() - 1, width - offsetxAmount, 0, offsetyAmount);
    }
    
    DeleteObject(hFont);
    //DeleteObject(color);
    EndPaint(hwnd, &ps);
}