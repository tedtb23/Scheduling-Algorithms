#include <Windows.h>
#include "SchedStats.hpp"
#include "Process.hpp"
#include "SimInfo.hpp"
#include "Render.hpp"
#include <CommCtrl.h>

static void displaySchedStat(const HDC& ctx, const SchedStats& stat, int offset, const int offsetyAmount) {
    std::string avgWait = "Avg Wait: " + std::to_string(stat.avgWaitTime);
    std::string avgTurnAround = "Avg TurnAround: " + std::to_string(stat.avgTurnAroundTime);
    //std::string avgBurst = "Avg Burst: " + std::to_string(stat.avgBurstTime);

    TextOutA(ctx, 0, offset, stat.AlgoUsed.c_str(), (int)stat.AlgoUsed.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgWait.c_str(), (int)avgWait.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgTurnAround.c_str(), (int)avgTurnAround.length());
    //offset += offsetyAmount;
    //TextOutA(ctx, 0, offset, avgBurst.c_str(), (int)avgBurst.length());
}

static void displaySchedStats(const HDC& ctx, const std::vector<SchedStats>& stats, const int startIdx, const int maxY, int offset, const int offsetyAmount) {
    for (int i = startIdx; i < stats.size(); i++) {
        const SchedStats& stat = stats[i];
        if (offset >= maxY) break;
        displaySchedStat(ctx, stat, offset, offsetyAmount);
        offset += (offsetyAmount * 4) + 10;
    }
}

static void displayProcesses(
    const HDC& ctx,
    const std::vector<Process>& processes,
    const long maxY,
    const size_t startIdx, const size_t endIdx, 
    const long startX, const long startY, 
    const int offsetyAmount) {

    size_t currIdx = startIdx;
    long offset = startY;
    //const int textHeight = (offsetyAmount * 4) + 10;

    while (currIdx <= endIdx && currIdx < processes.size()) {
        if (offset >= maxY) break;
        const Process& p = processes[currIdx];
        std::string pid = "PID: " + std::to_string(p.pid);
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

static void displayGanttChart(const HDC& ctx, const std::vector<GanttNode>& ganttChart, const size_t startIdx, const size_t endIdx, RECT rect) {
    SelectObject(ctx, GetStockObject(NULL_BRUSH)); //don't fill rects
    constexpr int nodeWidth = 50;
    constexpr int nodeHeight = 50;
    long width = rect.right - rect.left;
    long height = rect.bottom - rect.top;
    long x = rect.left;
    long y = rect.top;
    for (int i = startIdx; i <= endIdx; i++) {
        if (y >= rect.bottom) break;
        const GanttNode& gNode = ganttChart[i];
        std::string pid = std::to_string(gNode.p.pid);
        Rectangle(ctx, x, y, x + nodeWidth, y + nodeHeight);
        TextOutA(ctx, ((x + nodeWidth) + x) / 2, ((y + nodeHeight) + y) / 2, pid.c_str(), pid.length());
        x += nodeWidth;
        if (x >= rect.right) {
            x = rect.left;
            y += nodeHeight;
        }
    }
    DeleteObject(GetStockObject(NULL_BRUSH));
}

void UpdateSimStats(const HWND& hwnd, const SimInfo& simInfo) {
    constexpr int ganttOffset = 500;
    constexpr int offsetxAmount = 230;
    constexpr int offsetyAmount = 30;
    constexpr int prosTextHeight = (offsetyAmount * 4) + 10;
    constexpr int statTextHeight = prosTextHeight;
    int xPos; //xScroll position
    static int yPos; //yScroll position
    static int oldYPos;
    PAINTSTRUCT ps;
    SCROLLINFO si;
    HDC ctx = BeginPaint(hwnd, &ps);

    //for trackbar
    HWND hwndTrackbar;
    RECT trackbarRect;
    static int trackbarPos;
    ///

    //get window bounds
    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) {
        MessageBox(NULL, L"Failed to get window bounds.", L"Error", MB_ICONEXCLAMATION | MB_OK);
    }
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    ///
    
    //select in new font and color
    constexpr COLORREF textRGB = RGB(255, 255, 255);
    constexpr COLORREF bkRGB = RGB(30, 30, 30);
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
        displaySchedStats(ctx, simInfo.stats, yPos, bottom, 0, offsetyAmount);
    }

    for (SchedStats stat : simInfo.stats) {
        displayGanttChart(ctx, stat.ganttChart, 0, stat.ganttChart.size() - 1, { 0, 400, 800, height });
    }
    
    //display processes used text and trackbar pos
    if (yPos == 0) {
        std::string pTitle = "Processes Used : ";
        TextOutA(ctx, width - offsetxAmount - 180, 0, pTitle.c_str(), (int)pTitle.length());
        hwndTrackbar = FindWindowEx(hwnd, NULL, TRACKBAR_CLASS, L"Track Bar"); //get handle to child trackbar window.
        GetWindowRect(hwndTrackbar, &trackbarRect);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&trackbarRect, 2);
        trackbarPos = (int) SendMessage(hwndTrackbar, TBM_GETPOS, 0, 0);
        std::string trackBarPosStr = "Process Amount : " + std::to_string(trackbarPos);
        TextOutA(ctx, trackbarRect.left + 10, trackbarRect.bottom, trackBarPosStr.c_str(), (int)trackBarPosStr.length());
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
    EndPaint(hwnd, &ps);
}