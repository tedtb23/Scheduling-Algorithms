#include "FCFS.h"
#include <iostream>

SchedStats fcfs(std::vector<Process>& processes) {
	
	size_t n = processes.size();
	long clock = 0;
	long totalWaitTime = 0;
	long totalTurnAroundTime = 0;
	long totalBurstTime = 0;

	for (size_t i = 0; i < n; i++) {
		Process& pros = processes[i];

		if (clock >= pros.arrivalTime) {
			long startTime = clock;
			totalWaitTime += startTime - pros.arrivalTime;
			totalTurnAroundTime += (startTime + pros.totalBurst) - pros.arrivalTime;
			totalBurstTime += pros.totalBurst;
			clock = (startTime + pros.totalBurst);
		}
		else { //execute this if the arrivalTime of the next process has not been met. wait time omitted since it is always zero here.
			totalTurnAroundTime += pros.totalBurst;
			totalBurstTime += pros.totalBurst;
			clock = (pros.arrivalTime + pros.totalBurst);
		}
	}
	return {"FCFS", (totalWaitTime / (double)n), (totalTurnAroundTime / (double)n), (totalBurstTime / (double)n)};
}

