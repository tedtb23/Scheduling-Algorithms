#pragma once
#include "Process.hpp"
#include "SchedStats.hpp"
#include <vector>

SchedStats sjf(const std::vector<Process>& processes);