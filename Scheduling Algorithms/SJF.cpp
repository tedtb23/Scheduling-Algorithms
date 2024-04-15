#include "SJF.h"

SchedStats sjf(std::vector<Process>& processes) {
	std::vector<Process> ready;
	Process pros; //currently "executing" process.
	size_t i = 0;
	size_t n = processes.size();
	long startTime = LONG_MAX;
	long endTime = LONG_MAX;
	long minBurstTime = LONG_MAX;
	long clock = 0;

	long totalWaitTime = 0;
	long totalTurnAroundTime = 0;
	long totalBurstTime = 0;

	if(i < n) clock = processes[i].arrivalTime;
	while (true) {
		//put processes that have reached their arrival time into the ready queue.
		while (i < n && processes[i].arrivalTime <= clock) {
			ready.push_back(processes[i]);
			i++;
		}
		if (ready.size()) {
			//find minBurstTime and execute that process.
			for (size_t j = 0; j < ready.size(); j++) {
				if (minBurstTime > ready[j].totalBurstTime) {
					minBurstTime = ready[j].totalBurstTime;
					pros = ready[j];
				}
			}
			minBurstTime = LONG_MAX;
			auto newEnd = std::remove(ready.begin(), ready.end(), pros);
			ready.erase(newEnd, ready.end());
			startTime = clock;
			endTime = startTime + pros.totalBurstTime;
			totalWaitTime += startTime - pros.arrivalTime;
			totalTurnAroundTime += endTime - pros.arrivalTime;
			totalBurstTime += pros.totalBurstTime;
			clock = endTime;
		}
		
		//if we are out of unarrived and arrived processes, exit the loop.
		if (i >= n && !ready.size()) break;

		//move clock ahead if we are out of ready processes
		//and the last process's endTime did not put us at or past the arrivalTime of the next process.
		if (i < n && !ready.size() && clock < processes[i].arrivalTime) clock = processes[i].arrivalTime;
		
	}
	return { "SJF", (totalWaitTime / (long)n), (totalTurnAroundTime / (long)n), (totalBurstTime / (long)n) };
}