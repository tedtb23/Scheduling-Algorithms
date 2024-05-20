#pragma once
#include <string>
#include <vector>
#include "GanttNode.hpp"

struct SchedStats {
	std::string AlgoUsed;
	double avgWaitTime;
	double avgTurnAroundTime;
	size_t maxWaitTime;
	size_t maxTurnAroundTime;
	std::vector<GanttNode> ganttChart;
};