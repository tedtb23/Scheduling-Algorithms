#pragma once
#include "Process.h"
#include "SchedStats.h"
#include <vector>


SchedStats roundRobin(std::vector<Process>& processes, int timeQuantum);