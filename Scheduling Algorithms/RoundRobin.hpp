#pragma once
#include "Process.hpp"
#include "SchedStats.hpp"
#include <vector>


SchedStats roundRobin(const std::vector<Process>& processes, int timeQuantum);