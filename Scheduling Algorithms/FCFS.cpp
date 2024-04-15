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
			totalTurnAroundTime += (startTime + pros.totalBurstTime) - pros.arrivalTime;
			totalBurstTime += pros.totalBurstTime;
			clock = (startTime + pros.totalBurstTime);
		}
		else { //execute this if the arrivalTime of the next process has not been met. wait time omitted since it is always zero here.
			totalTurnAroundTime += pros.totalBurstTime;
			totalBurstTime += pros.totalBurstTime;
			clock = (pros.arrivalTime + pros.totalBurstTime);
		}
	}
	return {"FCFS", (totalWaitTime / (long)n), (totalTurnAroundTime / (long)n), (totalBurstTime / (long)n)};
}

