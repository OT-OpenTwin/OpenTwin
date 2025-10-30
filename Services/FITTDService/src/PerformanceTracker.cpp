// @otlicense
// File: PerformanceTracker.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
