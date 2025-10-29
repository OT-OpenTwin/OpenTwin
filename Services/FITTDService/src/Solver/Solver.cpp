// @otlicense

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

