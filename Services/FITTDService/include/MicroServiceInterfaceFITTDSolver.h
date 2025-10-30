// @otlicense
// File: MicroServiceInterfaceFITTDSolver.h
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
#include "MicroServiceSolver.h"
#include "EntityBase.h"

#include "PerformanceTracker.h"

#include "Solver/Solver.h"
#include "SolverSettings/PortSettings.h"
#include "SolverSettings/ResultPipelineSettings.h"
#include "SolverSettings/MonitorSettings.h"
#include "SolverSettings/SolverSettings.h"

#include "ResultFilter/ResultSink.h"
#include "ResultFilter/ResultSinkScalarAccumalating.h"
#include "ResultFilter/ResultSinkVector3DComplexSum.h"
#include "ResultFilter/ResultSinkScalarComplexSum.h"
#include "ResultFilter/ResultSinkVector3DAccumalating.h"

#include "Ports/Signal.h"

#include "Simulation/FITTD3DLoop.h"

#include <string>
#include "PlotBuilder.h"

//! @brief Interface between service API and solver functionality. Handles creation of entities created from solver results and settings ectrected from properties of ui entities.
class MicroServiceInterfaceFITTDSolver : public MicroServiceSolver
{
public:
	MicroServiceInterfaceFITTDSolver(std::string solverName, int serviceID, int sessionCount) :MicroServiceSolver(solverName, serviceID, sessionCount) {};
	virtual ~MicroServiceInterfaceFITTDSolver();

	void CreateSolver();
	void Run(void);

	void setDebugOption(bool flag) { debugOption = flag; }
	bool getDebugOption(void) const { return debugOption; }
private:
	PerformanceTracker _performanceTracker;

	bool debugOption = false;

	SolverSettings _solverSettings;
	std::vector<PortSettings> _portSettings;
	std::list<Signal> _signals;
	std::vector<MonitorSettings> _monitorSettings;

	ResultPipelineSettings * _resultPipelineSettings = nullptr;

	EntityMeshCartesianData * _mesh = nullptr;
	Solver * _solver = nullptr;

	void LoadEntityInformation(std::string solverName);
	void InitializeSolver(void);
	void HandleResultPipelines(void);
	
	void HandleTimelinePlots(const ResultSinkScalarAccumalating *resultSink, ResultPipeline * pipeline, PlotBuilder& _plotBuilder);
	void HandleTimelinePlots(const ResultSinkVector3DAccumalating *resultSink, ResultPipeline * pipeline, PlotBuilder& _plotBuilder);

	void SaveVectorFieldResult(const ResultSinkVector3DComplexSum *resultSink, ResultPipeline * pipeline);
	std::string CreateSolverRunDescription();
};
