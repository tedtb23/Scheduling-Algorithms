#pragma once
#include <string>
#include <vector>
#include "GanttNode.hpp"

struct SchedStats {
	std::string AlgoUsed;
	double avgWaitTime;
	double avgTurnAroundTime;
	long maxWaitTime;
	long maxTurnAroundTime;
	std::vector<GanttNode> ganttChart;
};