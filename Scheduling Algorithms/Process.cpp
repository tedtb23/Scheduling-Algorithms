#include "Process.h"
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

static bool compar(const Process& p1, const Process& p2) {
	return (p1.arrivalTime < p2.arrivalTime);
}

std::vector<Process> generateProcesses(int amount) {
	std::vector<Process> processes;
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);

	for (int i = 0; i < amount; i++) {
		int currPID = i;
		int currArrivalTime = gen() % 10000;
		int currBurstTime = gen() % 100;
		if (currBurstTime <= 0) currBurstTime = 1;
		int currPriority = gen() % 10;
		
		processes.push_back({ currPID, currArrivalTime, currBurstTime, currBurstTime, currPriority });
	}
	std::sort(processes.begin(), processes.end(), compar);

	return processes;
}