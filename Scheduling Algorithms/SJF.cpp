#include "SJF.hpp"
#include "SchedStats.hpp"
#include "Process.hpp"
#include "GanttNode.hpp"

SchedStats sjf(const std::vector<Process>& processes) {
	std::vector<GanttNode> ganttChart;
	std::vector<Process> ready;
	Process pros; //currently "executing" process.
	size_t i = 0;
	size_t n = processes.size();
	size_t startTime;
	size_t endTime;
	size_t waitTime;
	size_t turnAroundTime;
	int minBurstTime = LONG_MAX;
	size_t clock = 0;
	size_t totalWaitTime = 0;
	size_t totalTurnAroundTime = 0;
	size_t maxWaitTime = 0;
	size_t maxTurnAroundTime = 0;

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
				if (minBurstTime > ready[j].totalBurst) {
					minBurstTime = ready[j].totalBurst;
					pros = ready[j];
				}
			}
			minBurstTime = LONG_MAX;
			auto newEnd = std::remove(ready.begin(), ready.end(), pros);
			ready.erase(newEnd, ready.end());
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
		
		//if we are out of unarrived and arrived processes, exit the loop.
		if (i >= n && !ready.size()) break;

		//move clock ahead if we are out of ready processes
		//and the last process's endTime did not put us at or past the arrivalTime of the next process.
		if (i < n && !ready.size() && clock < processes[i].arrivalTime) clock = processes[i].arrivalTime;
		
	}
	return { "SJF", (totalWaitTime / (double)n), (totalTurnAroundTime / (double)n), maxWaitTime, maxTurnAroundTime, ganttChart };
}