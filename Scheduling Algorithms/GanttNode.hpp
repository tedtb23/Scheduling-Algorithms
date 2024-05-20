#pragma once
#include "Process.hpp"

struct GanttNode {
	Process p;
	size_t startTime;
	size_t endTime;
};