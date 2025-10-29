// @otlicense

#pragma once
#include "gtest/gtest.h"
#include "Grid/Grid.h"
#include "SystemDependencies/Allignments.h"
#include "Simulation/FITTD3DLoop.h"
#include "Solver/Solver.h"
#include "SolverSettings/SolverSettings.h"

class FixturePECBoundary : public testing::TestWithParam<int>
{
public:
	float * (*_getterPtr)(const DegreesOfFreedom3DLoopCPU<float> *doF, index_t index) = nullptr;
	~FixturePECBoundary();

	void CreateAirCuboidExCentredHardwiredSolver(Alignment alignment, index_t dimensionX);

	const Grid<float> * GetGrid() const { return _grid; };
	const index_t GetDimensionX() const { return _dimensionX; };
	Solver * GetSolver() const { return _solver; };
	FITTD3DLoop<float> * GetSimulator() const { return _simulator; };
	int GetSimulationDuration() const { return _simulationDuration; };

private:
	Grid<float> *_grid;
	index_t _dimensionX;
	FITTD3DLoop<float> * _simulator;
	Solver * _solver;
	SolverSettings _settings;

	int _simulationDuration = 120;
};