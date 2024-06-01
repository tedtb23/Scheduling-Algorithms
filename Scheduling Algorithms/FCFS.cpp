#include "FCFS.hpp"
#include "SchedStats.hpp"
#include "Process.hpp"
#include "GanttNode.hpp"
#include <iostream>

SchedStats fcfs(const std::vector<Process>& processes) {
	std::vector<GanttNode> ganttChart;
	size_t n = processes.size();
	size_t totalWaitTime = 0;
	size_t totalTurnAroundTime = 0;
	long clock = 0;
	long maxWaitTime = 0;
	long maxTurnAroundTime = 0;
	long startTime;
	long endTime;
	long waitTime;
	long turnAroundTime;

	for (size_t i = 0; i < n; i++) {
		const Process& pros = processes[i];

		if (clock >= pros.arrivalTime) {
			startTime = clock;
			endTime = startTime + pros.totalBurst;
			waitTime = startTime - pros.arrivalTime;
			turnAroundTime = endTime - pros.arrivalTime;

			if (waitTime >= maxWaitTime) maxWaitTime = waitTime;
			if (turnAroundTime >= maxTurnAroundTime) maxTurnAroundTime = turnAroundTime;
			ganttChart.push_back({ pros, startTime, endTime });
			totalWaitTime += waitTime;
			totalTurnAroundTime += turnAroundTime;
			clock = endTime;
		}
		else { //execute this if the arrivalTime of the next process has not been met. wait time omitted since it is always zero here.
			ganttChart.push_back({ pros, pros.arrivalTime, (pros.arrivalTime + pros.totalBurst) });
			totalTurnAroundTime += pros.totalBurst;
			clock = (pros.arrivalTime + pros.totalBurst);
		}
	}
	return {"FCFS", (totalWaitTime / (double)n), (totalTurnAroundTime / (double)n), maxWaitTime, maxTurnAroundTime, ganttChart};
}

