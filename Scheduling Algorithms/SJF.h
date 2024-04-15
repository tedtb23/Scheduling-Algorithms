#pragma once
#include "Process.h"
#include "SchedStats.h"
#include <vector>

SchedStats sjf(std::vector<Process>& processes);
