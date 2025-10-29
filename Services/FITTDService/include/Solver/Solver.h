// @otlicense

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
	std::vector<ResultPipeline*> _resultPipelines; //Könnte eine rP hinzufügen, welche den enthaltenen Energiewert berechnet. Diese wird in der Loop standardmäßig ausgeführt aber kann zu einer vorzeitigen abbruchbedingung führen.
	std::vector<Port<float>*> _ports;
	//Wenn  DegreesOfFreedom eine abstrakte API (kein DoF3D) erhalten, dann kann das ganze auch FITTD anstelle FITTD3D nutzen.
	FITTD3DLoop<float> * _simulation;
	unsigned int _simulationSteps;

	SolverSettings &_settings;
	SystemProperties _systemProperties = SystemProperties::GetInstance();

};