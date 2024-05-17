#include "RoundRobin.h"
#include <stdexcept>

SchedStats roundRobin(std::vector<Process>& processes, int timeQuantum) {
	if (timeQuantum <= 0) throw std::invalid_argument("Time quantum must be greater than zero");

	std::vector<Process> ready;
	Process pros; //currently "executing" process.
	size_t i = 0;
	size_t n = processes.size();
	long startTime = LONG_MAX;
	long endTime = LONG_MAX;
	long clock = 0;

	long totalWaitTime = 0;
	long totalTurnAroundTime = 0;
	long totalBurstTime = 0;

	if (i < n) clock = processes[i].arrivalTime;
	while (true) {
		//put processes that have reached their arrival time into the ready queue.
		while (i < n && processes[i].arrivalTime <= clock) {
			ready.push_back(processes[i]);
			i++;
		}

		//finish me
		if (ready.size()) {
			pros = ready[0];
			ready.erase(ready.begin());
			startTime = clock;
			if (pros.remainingBurst > timeQuantum) {
				endTime = startTime + timeQuantum;
				pros.remainingBurst -= timeQuantum;
				ready.push_back(pros);
			}
			else {
				endTime = startTime + pros.remainingBurst;
				pros.remainingBurst = 0;
				totalTurnAroundTime += endTime - pros.arrivalTime;
				totalWaitTime += (endTime - pros.arrivalTime) - pros.totalBurst;
				totalBurstTime += pros.totalBurst;
			}
			clock = endTime;
		}

		//if we are out of unarrived and arrived processes, exit the loop.
		if (i >= n && !ready.size()) break;

		//move clock ahead if we are out of ready processes
		//and the last process's endTime did not put us at or past the arrivalTime of the next process.
		if (i < n && !ready.size() && clock < processes[i].arrivalTime) clock = processes[i].arrivalTime;

	}
	return { "Round Robin", (totalWaitTime / (double)n), (totalTurnAroundTime / (double)n), (totalBurstTime / (double)n) };
}