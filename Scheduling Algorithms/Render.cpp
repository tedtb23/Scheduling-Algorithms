#include <Windows.h>
#include "SchedStats.hpp"
#include "Process.hpp"
#include "SimInfo.hpp"
#include "Render.hpp"
#include <sstream>
#include <CommCtrl.h>
#include <iomanip>

static inline void displaySchedStat(const HDC& ctx, const SchedStats& stat, int offset, const int offsetyAmount) {
    std::string algoStat = stat.AlgoUsed + " Stats : ";

    std::stringstream avgWaitStream;
    avgWaitStream << std::fixed << std::setprecision(2) << stat.avgWaitTime;
    std::string avgWaitStrVal = avgWaitStream.str();
    std::string avgWait = "Avg Wait: " + avgWaitStrVal;

    std::stringstream avgTurnAroundStream;
    avgTurnAroundStream << std::fixed << std::setprecision(2) << stat.avgTurnAroundTime;
    std::string avgTurnAroundStrVal = avgTurnAroundStream.str();
    std::string avgTurnAround = "Avg TurnAround: " + avgTurnAroundStrVal;

    std::stringstream maxWaitStream;
    maxWaitStream << std::fixed << std::setprecision(2) << stat.maxWaitTime;
    std::string maxWaitStrVal = maxWaitStream.str();
    std::string maxWait = "Max Wait: " + maxWaitStrVal;

    TextOutA(ctx, 0, offset, algoStat.c_str(), (int)algoStat.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgWait.c_str(), (int)avgWait.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, avgTurnAround.c_str(), (int)avgTurnAround.length());
    offset += offsetyAmount;
    TextOutA(ctx, 0, offset, maxWait.c_str(), (int)maxWait.length());
}

static inline void displaySchedStats(const HDC& ctx, const std::vector<SchedStats>& stats, const int startIdx, const int maxY, int offset, const int offsetyAmount) {
    for (int i = startIdx; i < stats.size(); i++) {
        const SchedStats& stat = stats[i];
        if (offset >= maxY) break;
        displaySchedStat(ctx, stat, offset, offsetyAmount);
        offset += (offsetyAmount * 4) + 10;
    }
}

static inline void displayProcesses(
    const HDC& ctx,
    const std::vector<Process>& processes,
    const long maxY,
    const size_t startIdx,
    const long startX, const long startY, 
    const int offsetyAmount) {

    size_t currIdx = startIdx;
    long offset = startY;

    while (currIdx < processes.size()) {
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

static inline long displayGanttChart(
    const HDC& ctx, 
    const std::string& algoUsed, 
    const std::vector<GanttNode>& ganttChart, 
    const RECT& rect) {

    SelectObject(ctx, GetStockObject(NULL_BRUSH)); //don't fill rects
    constexpr int nodeWidth = 70;
    constexpr int nodeHeight = 50;
    const long width = rect.right - rect.left;
    const long height = rect.bottom - rect.top;
    long x = rect.left;
    long y = rect.top;
    size_t i;
    std::string algoUsedGantt = algoUsed + " Gantt Chart : ";
    TextOutA(ctx, x, y, algoUsedGantt.c_str(), (int)algoUsedGantt.length());
    y += 30;
    for (i = 0; i < ganttChart.size(); i++) {

        //if we have met or exceeded the bottom bound of rect, stop displaying nodes.
        if (y >= rect.bottom) break;

        const GanttNode& gNode = ganttChart[i];
        std::string pid = std::to_string(gNode.p.pid);
        std::string startTime = std::to_string(gNode.startTime);
        std::string endTime = std::to_string(gNode.endTime);

        //display node with pid and starttime.
        Rectangle(ctx, x, y, x + nodeWidth, y + nodeHeight);
        TextOutA(ctx, (nodeWidth / 4) + x, (nodeHeight / 4) + y, pid.c_str(), (int)pid.length());
        TextOutA(ctx, x, y + nodeHeight, startTime.c_str(), (int)startTime.length());

        x += nodeWidth;
        //display endtime too if this is the last node.
        if(i == ganttChart.size() - 1) TextOutA(ctx, x, y + nodeHeight, endTime.c_str(), (int)endTime.length());

        //if the next node will put us outside the right bound of rect. go to new line
        if (x + nodeWidth > rect.right) {
            //display endtime if this is the last node in a line.
            TextOutA(ctx, x, y + nodeHeight, endTime.c_str(), (int)endTime.length());
            x = rect.left;
            y += nodeHeight + 30;
        }
    }
    DeleteObject(GetStockObject(NULL_BRUSH));
    return y;
}

static inline void displayGanttCharts(
    const HDC& ctx,
    const std::vector<SchedStats>& stats,
    RECT* rectPtr) {
    long y;

    for (SchedStats stat : stats) {
         y = displayGanttChart(
            ctx,
            stat.AlgoUsed,
            stat.ganttChart,
            *rectPtr);
         y += 100;
         if (y < rectPtr->bottom) {
             rectPtr->top = y;
         }
         else break;
    }
}

static inline HWND getTrackbar(const HWND& hwnd, const std::string& trackbarTitle) {
    std::wstring stemp = std::wstring(trackbarTitle.begin(), trackbarTitle.end());
    LPCWSTR trackbarTitleWide = stemp.c_str();
    return FindWindowEx(hwnd, NULL, TRACKBAR_CLASS, trackbarTitleWide); //get handle to child trackbar window.
}

static RECT getTrackbarRect(const HWND& hwnd, const std::string& trackbarTitle) {
    RECT trackbarRect;
    HWND hwndTrackbar = getTrackbar(hwnd, trackbarTitle);
    if (!GetWindowRect(hwndTrackbar, &trackbarRect)) {
        MessageBox(NULL, L"Failed to get trackbar bounds.", L"Error", MB_ICONEXCLAMATION | MB_OK);
    }
    MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&trackbarRect, 2);
    return trackbarRect;
}

static inline void displayTrackbarPos(const HWND& hwnd, const HDC& ctx, const std::string& trackbarTitle, const std::string& posTitle) {
    int pos;
    HWND hwndTrackbar = getTrackbar(hwnd, trackbarTitle);
    RECT trackbarRect = getTrackbarRect(hwnd, trackbarTitle);
    pos = (int)SendMessage(hwndTrackbar, TBM_GETPOS, 0, 0);
    std::string trackBarPosStr = posTitle + std::to_string(pos);
    TextOutA(ctx, trackbarRect.left + 5, trackbarRect.bottom, trackBarPosStr.c_str(), (int)trackBarPosStr.length());
}

void UpdateSimStats(const HWND& hwnd, const SimInfo& simInfo) {
    constexpr int offsetxAmount = 230;
    constexpr int offsetyAmount = 30;
    static int yPos; //yScroll position
    //static int oldYPos;
    PAINTSTRUCT ps;
    SCROLLINFO si{};
    HDC ctx = BeginPaint(hwnd, &ps);

    //get window bounds
    RECT clientRect;
    if (!GetClientRect(hwnd, &clientRect)) {
        MessageBox(NULL, L"Failed to get client bounds.", L"Error", MB_ICONEXCLAMATION | MB_OK);
    }
    const int width = clientRect.right;
    const int height = clientRect.bottom;
    ///

    //get gantt chart rect
    const long ganttTop = getTrackbarRect(hwnd, "Time Quantum Trackbar").bottom + 30;
    RECT ganttRect = { 0, ganttTop, 600, height };
    ///
    
    //select in new font and color
    constexpr COLORREF textRGB = RGB(255, 255, 255); //white
    constexpr COLORREF bkRGB = RGB(30, 30, 30); //grey
    const COLORREF oldBKColor = SetBkColor(ctx, bkRGB);
    const COLORREF oldTextColor = SetTextColor(ctx, textRGB);
    const HFONT hFont = CreateFontA(25, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    const HFONT hOldFont = (HFONT)SelectObject(ctx, hFont);
    ///

    //get update region and scroll info
    LONG top = ps.rcPaint.top;
    LONG left = ps.rcPaint.left;
    LONG bottom = ps.rcPaint.bottom;
    LONG right = ps.rcPaint.right;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    //oldYPos = yPos;
    yPos = si.nPos;
    ///
    
    //display processes used text and trackbar pos
    if (yPos == 0) {
        std::string pTitle = "Processes Used : ";
        TextOutA(ctx, width - offsetxAmount - 180, 0, pTitle.c_str(), (int)pTitle.length());
    }
    displayTrackbarPos(hwnd, ctx, "Processes Trackbar", "Process Amount : ");
    displayTrackbarPos(hwnd, ctx, "Time Quantum Trackbar", "RR Time Quantum : ");

    //display stats in updated area
    if (yPos < simInfo.stats.size() && yPos >= 0) {
        displaySchedStats(ctx, simInfo.stats, yPos, bottom, 0, offsetyAmount);
    }

    //display gantt charts
    displayGanttCharts(ctx, simInfo.stats, &ganttRect);
    
    //paint processes in updated area
    displayProcesses(ctx, simInfo.processes, height, yPos, width - offsetxAmount, 0, offsetyAmount);
    
    DeleteObject(hFont);
    EndPaint(hwnd, &ps);
}