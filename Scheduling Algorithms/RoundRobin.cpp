#include "RoundRobin.hpp"
#include "SchedStats.hpp"
#include "Process.hpp"
#include "GanttNode.hpp"
#include <stdexcept>

SchedStats roundRobin(const std::vector<Process>& processes, int timeQuantum) {
	if (timeQuantum <= 0) throw std::invalid_argument("Time quantum must be greater than zero");

	std::vector<GanttNode> ganttChart;
	std::vector<Process> ready;
	Process pros; //currently "executing" process.
	size_t i = 0;
	size_t n = processes.size();
	size_t startTime = LONG_MAX;
	size_t endTime = LONG_MAX;
	size_t clock = 0;
	size_t totalWaitTime = 0;
	size_t totalTurnAroundTime = 0;
	size_t maxWaitTime = 0;
	size_t maxTurnAroundTime = 0;

	if (i < n) clock = processes[i].arrivalTime;
	while (true) {
		//put processes that have reached their arrival time into the ready queue.
		while (i < n && processes[i].arrivalTime <= clock) {
			ready.insert(ready.begin(), processes[i]);
			i++;
		}

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
				size_t turnAroundTime = endTime - pros.arrivalTime;
				size_t waitTime = turnAroundTime - pros.totalBurst;
				pros.remainingBurst = 0;
				totalTurnAroundTime += turnAroundTime;
				totalWaitTime += waitTime;
				if (waitTime >= maxWaitTime) maxWaitTime = waitTime;
				if (turnAroundTime >= maxTurnAroundTime) maxTurnAroundTime = turnAroundTime;
			}
			ganttChart.push_back({ pros, startTime, endTime });
			clock = endTime;
		}

		//if we are out of unarrived and arrived processes, exit the loop.
		if (i >= n && !ready.size()) break;

		//move clock ahead if we are out of ready processes
		//and the last process's endTime did not put us at or past the arrivalTime of the next process.
		if (i < n && !ready.size() && clock < processes[i].arrivalTime) clock = processes[i].arrivalTime;

	}
	return { "Round Robin", (totalWaitTime / (double)n), (totalTurnAroundTime / (double)n), maxWaitTime, maxTurnAroundTime, ganttChart };
}