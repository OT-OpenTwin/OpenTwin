// @otlicense
// File: Solver.cpp
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

#include "Solver/Solver.h"

Solver::~Solver()
{
	for (auto port : _ports)
	{
		delete port;
		port = nullptr;
	}
	for (auto resultPipeline : _resultPipelines)
	{
		delete resultPipeline;
		resultPipeline = nullptr;
	}
	delete _simulation;
	_simulation = nullptr;
}

void Solver::SimulateMultipleTimesteps()
{
	float time = _simulation->GetTimeDiscretization();
#pragma omp parallel 
	{
		_settings.SetActiveNumberOfThreads(_systemProperties.GetActiveNbOfThreads());
		for (unsigned int timeStep = 0; timeStep < _simulationSteps; timeStep++)
		{

			_simulation->UpdateSimulationValuesH();
#pragma omp barrier
			_simulation->UpdateSimulationValuesE();
#pragma omp barrier
#pragma omp single //Has implicit barrier
			{
				//Has to be before the resultpipeline. Assert in resultpipeline checks for equal timestep in port and resultpipeline
				for (auto port : _ports)
				{
					port->ApplySource(*_simulation->GetDegreesOfFreedom(), timeStep, time);
				}

				for (auto resultPipeline : _resultPipelines)
				{
					resultPipeline->Execute(timeStep);
				}
				time += _simulation->GetTimeDiscretization();
			}
		}
	}
}

void Solver::ExecuteOneTimeStepNoParallelization(int currentTimestep)
{
	float time = _simulation->GetTimeDiscretization() * currentTimestep;
	_simulation->UpdateSimulationValuesE();
	_simulation->UpdateSimulationValuesH();
	for (auto port : _ports)
	{
		port->ApplySource(*_simulation->GetDegreesOfFreedom(), currentTimestep,time);
	}
	for (auto resultPipeline : _resultPipelines)
	{
		resultPipeline->Execute(currentTimestep);
	}
}

