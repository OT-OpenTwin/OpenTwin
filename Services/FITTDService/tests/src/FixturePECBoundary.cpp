// @otlicense
// File: FixturePECBoundary.cpp
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

#include "FixturePECBoundary.h"
#include "CurlCoefficients/Coefficient3DFactory.h"
#include "Simulation/FITTD3DBuilder.h"
#include "Ports/PortGaussian.h"

FixturePECBoundary::~FixturePECBoundary()
{
	delete _grid;
	delete _solver;
}

void FixturePECBoundary::CreateAirCuboidExCentredHardwiredSolver(Alignment alignment, index_t dimensionX)
{
	_dimensionX = dimensionX;
	index_t degreesOfFreedomInY(13), degreesOfFreedomInZ(13);

	_grid = new Grid<float>(_dimensionX, degreesOfFreedomInY, degreesOfFreedomInZ, alignment);

	auto coefficients = coefficient3DFactory.CreateUniformAirCoefficientsSP(*_grid);
	auto coefficientsDual = coefficient3DFactory.CreateUniformAirDualCoefficientsSP(*_grid);

	fittd3DBuilder.AddGridSP(_grid);
	fittd3DBuilder.AddCurlCoefficientsSP(coefficients);
	fittd3DBuilder.AddCurlCoefficientsDualSP(coefficientsDual);

	int simulationSteps = 3;


	_simulator = fittd3DBuilder.CreateSimulationSP();
	index_t x(_dimensionX / 2), y(degreesOfFreedomInY / 2), z(degreesOfFreedomInZ / 2);
	index_t sourceIndex = _grid->GetIndexToCoordinate(x,y ,z );

	std::pair<Point3D, index_t> portLocation({ x,y,z }, sourceIndex);
	std::vector< Port<float>* > ports{ new PortGaussian<float>("FixturePort", portLocation, e, x_axis,"TestRun" ,30, 100 )
};
	std::vector< ResultPipeline*> resultPipelines;
	_solver = new Solver(simulationSteps, ports, resultPipelines, _simulator, _settings);
}