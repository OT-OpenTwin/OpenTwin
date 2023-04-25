#include "PerformanceTracker.h"

std::string PerformanceTracker::Print()
{
	std::string performanceDescription =
		"Time for setup: " + std::to_string(_setupTime) + "s\n" +
		"Time for simulation: " + std::to_string(_simulationTime) + "s\n" +
		"Number of simulated nodes: " + std::to_string(_numberOfNodes) + "\n";

	return performanceDescription;
}

void PerformanceTracker::StartTimer()
{
	_start = clock();
}

double PerformanceTracker::StopTimer()
{
	clock_t end = clock();
	return (end - _start) / CLOCKS_PER_SEC;
}
