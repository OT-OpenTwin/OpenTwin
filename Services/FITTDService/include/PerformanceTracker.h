#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include <time.h>
#include <string>


class PerformanceTracker
{
public:
	std::string Print(void);
	void StartTimer(void);
	double StopTimer(void);
	void SetSimulationTime(double time) { _simulationTime = time; }
	void SetSetupTime(double time) { _setupTime = time; }
	void SetNumberOfNodes(index_t nodes) { _numberOfNodes = nodes; }

private:
	double _setupTime;
	double _simulationTime;
	index_t _numberOfNodes;

	clock_t _start;
};
