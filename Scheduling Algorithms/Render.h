#pragma once
#include <Windows.h>
#include "SimInfo.h"
#include "Process.h"
#include "SchedStats.h"

static void displaySchedStat(const HDC& ctx, const SchedStats& stat, int offset, const int offsetyAmount);

static void displaySchedStats(const HDC& ctx, const std::vector<SchedStats>& stats, const int height, int offset, const int offsetyAmount);

static void displayProcesses(
    const HDC& ctx,
    const std::vector<Process>& processes,
    const int height,
    const int startIdx, const int endIdx,
    const int startX, const int startY,
    const int offsetyAmount);

void UpdateSimStats(const HWND& hwnd, const SimInfo& simInfo);