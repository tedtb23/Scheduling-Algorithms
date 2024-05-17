#pragma once
#include "SchedSim.h"
#include "SchedStats.h"
#include "Process.h"
#include "FCFS.h"
#include "SJF.h"
#include "RoundRobin.h"
#include <vector>
#include <iostream>
#include <string>
#include <memory>

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
		std::cout << currAlgo << " AVG BURST TIME: " << stats[i].avgBurstTime << std::endl;
		printEquals(50);
	}
}

SimInfo simulate(int amount) {
	//fix me
	//std::unique_ptr<std::vector<SchedStats>> statsPtr = std::make_unique<std::vector<SchedStats>>();
	//std::unique_ptr<std::vector<Process>> processesPtr = std::make_unique<std::vector<Process>>();
	//std::unique_ptr<SimInfo> simInfoPtr = std::make_unique<SimInfo>(*processesPtr, *statsPtr);
	//std::vector<Process>& processes = simInfoPtr->processes;
	//std::vector<SchedStats>& stats = simInfoPtr->stats;
	SimInfo simInfo;
	std::vector<SchedStats>& stats = simInfo.stats;
	std::vector<Process>& processes = simInfo.processes;

	processes = generateProcesses(amount);
	stats.push_back(fcfs(processes));
	stats.push_back(sjf(processes));
	stats.push_back(roundRobin(processes, 2));
	//stats.push_back(priority(processesPtr));
	//stats.push_back(multiLevelQueue(processesPtr));
	//stats.push_back(multiLevelFeedbackQueue(processesPtr));

	//printStats(stats, processes);

	return simInfo;
}

