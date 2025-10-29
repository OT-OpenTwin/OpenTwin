// @otlicense

#pragma once
#include <gtest/gtest.h>

#include "Solver/Solver.h"
#include "Simulation/FITTD3DLoop.h"
#include "SystemDependencies/Allignments.h"
#include "Grid/Grid.h"
#include "SolverSettings/SolverSettings.h"

class FixtureSimulation3D : public testing::TestWithParam<Alignment>
{
public:
	~FixtureSimulation3D();

	void CreateAirCubeExCentredHardwiredSolver(Alignment alignment);
	void CreateAirCubeEyCentredHardwiredSolver(Alignment alignment);
	void CreateAirCubeHxCentredHardwiredSolver(Alignment alignment);
	void CreateAirCubeHyCentredHardwiredSolver(Alignment alignment);

	const index_t GetSourceIndex() const { return _sourceIndex; };
	const index_t GetSourceCoordinateX() const { return _sourceCoordinateX; };
	const index_t GetSourceCoordinateY() const { return _sourceCoordinateY; };
	const index_t GetSourceCoordinateZ() const { return _sourceCoordinateZ; };

	FITTD3DLoop<float> * GetSimulator() const { return _simulation; };

	const index_t GetDegreesOfFreedomInX()const { return _degreesOfFreedomInX; };
	const index_t GetDegreesOfFreedomInY()const { return _degreesOfFreedomInY; };
	const index_t GetDegreesOfFreedomInZ()const { return _degreesOfFreedomInZ; };

	void SimulateTimestep(int timeStep);
private:

	index_t _sourceIndex;
	index_t _sourceCoordinateX;
	index_t _sourceCoordinateY;
	index_t _sourceCoordinateZ;

	index_t _degreesOfFreedomInX;
	index_t  _degreesOfFreedomInY;
	index_t  _degreesOfFreedomInZ;

	FITTD3DLoop<float> * _simulation = nullptr;
	Grid<float> * _grid = nullptr;
	CurlCoefficients3D<float> * _coefficients = nullptr;
	CurlCoefficients3D<float> * _coefficientsDual = nullptr;
	Port<float> * _port = nullptr;

	void CreateAirCubeECentredHardwiredSolver(sourceType sourceType, axis sourceAxis, Alignment alignment);
};