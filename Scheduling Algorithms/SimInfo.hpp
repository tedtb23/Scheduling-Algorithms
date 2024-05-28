#pragma once
#include <vector>
#include "SchedStats.hpp"
#include "Process.hpp"

struct SimInfo {
	std::vector<SchedStats> stats;
	std::vector<Process> processes;
	int minProcesses;
	int maxProcesses;
	int minTQ;
	int maxTQ;
};