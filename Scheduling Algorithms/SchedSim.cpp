#include "SchedSim.hpp"
#include "SchedStats.hpp"
#include "Process.hpp"
#include "FCFS.hpp"
#include "SJF.hpp"
#include "RoundRobin.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

static void printEquals(int amount) {
	for (int i = 0; i < amount; i++) std::cout << "=";
	std::cout << std::endl << std::endl;
}

static void printProcesses(std::vector<Process>& processes) {
	printEquals(50);
	std::cout << "PROCESS LIST: " << std::endl << std::endl;
	for (size_t i = 0; i < processes.size(); i++) {
		std::cout << "PROCESS PID: " << processes[i].pid << std::endl;
		std::cout << "PROCESS ARRIVAL TIME: " << processes[i].arrivalTime << std::endl;
		std::cout << "PROCESS BURST TIME: " << processes[i].totalBurst << std::endl;
		std::cout << "PROCESS PRIORITY: " << processes[i].priority << std::endl << std::endl;
	}

	std::cout << "PROCESS ARRIVAL TIMES FOR COPYING: " << std::endl << std::endl;
	for (size_t i = 0; i < processes.size(); i++) {
		std::cout << processes[i].arrivalTime << " ";
	}

	std::cout << std::endl << std::endl << "PROCESS BURST TIMES FOR COPYING: " << std::endl << std::endl;
	for (size_t i = 0; i < processes.size(); i++) {
		std::cout << processes[i].totalBurst << " ";
	}
	std::cout << std::endl;
	printEquals(50);
}

static void printStats(std::vector<SchedStats>& stats, std::vector<Process>& processes) {
	printProcesses(processes);
	
	for (size_t i = 0; i < stats.size(); i++) {
		printEquals(50);
		std::string currAlgo = stats[i].AlgoUsed;
		std::cout << currAlgo << " AVG WAIT TIME: " << stats[i].avgWaitTime << std::endl;
		std::cout << currAlgo << " AVG TURN AROUND TIME : " << stats[i].avgTurnAroundTime << std::endl;
		//std::cout << currAlgo << " AVG BURST TIME: " << stats[i].avgBurstTime << std::endl;
		printEquals(50);
	}
}

SimInfo simulate(int amount, int timeQuantum) {
	constexpr int maxProcesses = 100;
	constexpr int minProcesses = 10;
	constexpr int maxTQ = 100;
	constexpr int minTQ = 1;
	if(amount > maxProcesses) throw std::invalid_argument("processes specified is greater than maximum");
	if(amount < minProcesses) throw std::invalid_argument("processes specified is less than minimum");
	if (timeQuantum > maxTQ) throw std::invalid_argument("time quantum specified is greater than maximum");
	if (timeQuantum < minTQ) throw std::invalid_argument("time quantum specified is less than minimum");
	SimInfo simInfo;
	simInfo.minProcesses = minProcesses;
	simInfo.maxProcesses = maxProcesses;
	simInfo.minTQ = minTQ;
	simInfo.maxTQ = maxTQ;
	std::vector<SchedStats>& stats = simInfo.stats;
	std::vector<Process>& processes = simInfo.processes;

	processes = generateProcesses(amount);
	stats.push_back(fcfs(processes));
	stats.push_back(sjf(processes));
	stats.push_back(roundRobin(processes, timeQuantum));
	//stats.push_back(priority(processesPtr));
	//stats.push_back(multiLevelQueue(processesPtr));
	//stats.push_back(multiLevelFeedbackQueue(processesPtr));

	//printStats(stats, processes);

	return simInfo;
}

