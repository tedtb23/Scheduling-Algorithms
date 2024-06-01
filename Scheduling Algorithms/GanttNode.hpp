#pragma once
#include "Process.hpp"

struct GanttNode {
	Process p;
	long startTime;
	long endTime;
};