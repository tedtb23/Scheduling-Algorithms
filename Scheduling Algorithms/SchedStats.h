#pragma once
#include <string>

struct SchedStats {
	std::string AlgoUsed;
	double avgWaitTime;
	//int minWaitTime;
	//int maxWaitTime;
	double avgTurnAroundTime;
	//int minTurnAroundTime;
	//int maxTurnAroundTime;
	double avgBurstTime;
};