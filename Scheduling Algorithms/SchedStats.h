#pragma once
#include <string>

struct SchedStats {
	std::string AlgoUsed;
	int avgWaitTime;
	//int minWaitTime;
	//int maxWaitTime;
	int avgTurnAroundTime;
	//int minTurnAroundTime;
	//int maxTurnAroundTime;
	int avgBurstTime;
};