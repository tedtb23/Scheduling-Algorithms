#pragma once
#include <vector>

struct Process {
	int pid;
	int arrivalTime;
	int remainingBurst;
	int totalBurst;
	int priority;

	bool operator==(const Process& other) const {
		return pid == other.pid;
	}
};

std::vector<Process> generateProcesses(int amount);