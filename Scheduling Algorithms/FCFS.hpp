#pragma once
#include "Process.hpp"
#include "SchedStats.hpp"
#include <vector>

SchedStats fcfs(const std::vector<Process>& processes);