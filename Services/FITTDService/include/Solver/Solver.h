// @otlicense
// File: Solver.h
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

#pragma once
#include <vector>
#include <string>

#include "ResultPipeline/ResultPipeline.h"
#include "Ports/Port.h"
#include "Simulation/FITTD3DLoop.h"
#include "SystemDependencies/SystemProperties.h"
#include "SolverSettings/SolverSettings.h"

class Solver
{
public:
	Solver(unsigned int simulationSteps, std::vector<Port<float>*> ports, std::vector<ResultPipeline*> resultPipelines, FITTD3DLoop<float>* simulation, SolverSettings &settings)
		:_simulationSteps(simulationSteps), _ports(ports), _resultPipelines(resultPipelines), _simulation(simulation), _settings(settings)
	{};
	Solver(Solver & other) = delete;
	Solver & operator=(Solver & other) = delete;
	~Solver();
	
	std::vector<ResultPipeline*> GetResultPipelines() const { return _resultPipelines; }

	void SimulateMultipleTimesteps();
	void ExecuteOneTimeStepNoParallelization(int currentTimestep);

private:
	std::vector<ResultPipeline*> _resultPipelines; //K�nnte eine rP hinzuf�gen, welche den enthaltenen Energiewert berechnet. Diese wird in der Loop standardm��ig ausgef�hrt aber kann zu einer vorzeitigen abbruchbedingung f�hren.
	std::vector<Port<float>*> _ports;
	//Wenn  DegreesOfFreedom eine abstrakte API (kein DoF3D) erhalten, dann kann das ganze auch FITTD anstelle FITTD3D nutzen.
	FITTD3DLoop<float> * _simulation;
	unsigned int _simulationSteps;

	SolverSettings &_settings;
	SystemProperties _systemProperties = SystemProperties::GetInstance();

};