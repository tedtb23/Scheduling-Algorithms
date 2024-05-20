#pragma once
#include <Windows.h>
#include "SimInfo.hpp"
#include "Process.hpp"
#include "SchedStats.hpp"

static void displaySchedStat(const HDC& ctx, const SchedStats& stat, int offset, const int offsetyAmount);

static void displaySchedStats(const HDC& ctx, const std::vector<SchedStats>& stats, const int startIdx, const int maxY, int offset, const int offsetyAmount);

static void displayProcesses(
    const HDC& ctx,
    const std::vector<Process>& processes,
    const long maxY,
    const size_t startIdx, const size_t endIdx,
    const long startX, const long startY,
    const int offsetyAmount);

void UpdateSimStats(const HWND& hwnd, const SimInfo& simInfo);