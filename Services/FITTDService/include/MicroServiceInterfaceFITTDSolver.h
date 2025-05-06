/*********************************************************************
 * @file   MicroServiceInterfaceFITTDSolver.h
 * @brief  Interface between service API and solver functionality. Handles creation of entities created from solver results and settings ectrected from properties of ui entities.
 * 
 * @author Jan Wagner
 * @date   July 2022
 *********************************************************************/
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

class ClassFactory;

class MicroServiceInterfaceFITTDSolver : public MicroServiceSolver
{
public:
	MicroServiceInterfaceFITTDSolver(std::string solverName, int serviceID, int sessionCount, ClassFactory &classFactory) :MicroServiceSolver(solverName, serviceID, sessionCount, classFactory) {};
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
	
	void HandleTimelinePlots(const ResultSinkScalarAccumalating *resultSink, ResultPipeline * pipeline);
	void HandleTimelinePlots(const ResultSinkVector3DAccumalating *resultSink, ResultPipeline * pipeline);

	void SaveVectorFieldResult(const ResultSinkVector3DComplexSum *resultSink, ResultPipeline * pipeline);
	std::string CreateSolverRunDescription();
};
