#pragma once
#include <vector>
#include "SchedStats.h"
#include "Process.h"
#include <Windows.h>

struct SimInfo {
	std::vector<SchedStats> stats;
	std::vector<Process> processes;
};