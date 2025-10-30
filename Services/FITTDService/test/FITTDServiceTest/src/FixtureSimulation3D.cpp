// @otlicense
// File: FixtureSimulation3D.cpp
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

#include "FixtureSimulation3D.h"
#include "Grid/Grid.h"
#include "CurlCoefficients/Coefficient3DFactory.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include "Simulation/FITTD3DBuilder.h"
#include "SystemDependencies/Allignments.h"
#include "Ports/PortGaussian.h"
#include "ResultPipeline/ResultPipeline.h"




FixtureSimulation3D ::~FixtureSimulation3D()
{
	delete _simulation;
	delete _port;
}

void FixtureSimulation3D::CreateAirCubeEyCentredHardwiredSolver(Alignment alignment)
{
	CreateAirCubeECentredHardwiredSolver(e,y_axis, alignment);
}

void FixtureSimulation3D::CreateAirCubeExCentredHardwiredSolver(Alignment alignment)
{
	CreateAirCubeECentredHardwiredSolver(e,x_axis, alignment);
}


void FixtureSimulation3D::CreateAirCubeHxCentredHardwiredSolver(Alignment alignment)
{
	CreateAirCubeECentredHardwiredSolver(h, x_axis, alignment);
}

void FixtureSimulation3D::CreateAirCubeHyCentredHardwiredSolver(Alignment alignment)
{
	CreateAirCubeECentredHardwiredSolver(h, y_axis, alignment);
}

void FixtureSimulation3D::SimulateTimestep(int timeStep)
{
	_simulation->UpdateSimulationValuesE();
	_simulation->UpdateSimulationValuesH();

	_port->ApplySourceHWDiscreteValueCalc(*_simulation->GetDegreesOfFreedom(), timeStep);
}

void FixtureSimulation3D::CreateAirCubeECentredHardwiredSolver(sourceType sourceType, axis sourceAxis, Alignment alignment)
{

	_degreesOfFreedomInX = _degreesOfFreedomInY = _degreesOfFreedomInZ = 13;

	_grid = new Grid<float>(_degreesOfFreedomInX, _degreesOfFreedomInY, _degreesOfFreedomInZ, alignment);
	_coefficients = coefficient3DFactory.CreateUniformAirCoefficientsSP(*_grid);
	_coefficientsDual = coefficient3DFactory.CreateUniformAirDualCoefficientsSP(*_grid);

	fittd3DBuilder.AddGridSP(_grid);
	fittd3DBuilder.AddCurlCoefficientsSP(_coefficients);
	fittd3DBuilder.AddCurlCoefficientsDualSP(_coefficientsDual);
	_simulation = fittd3DBuilder.CreateSimulationSP();

	int simulationSteps = 3;
	_sourceCoordinateX = _sourceCoordinateY = _sourceCoordinateZ = (_degreesOfFreedomInX / 2);
	_sourceIndex = _grid->GetIndexToCoordinate( _sourceCoordinateX,_sourceCoordinateY,_sourceCoordinateZ);

	std::pair<Point3D, index_t> portLocation({ _sourceCoordinateX,_sourceCoordinateY,_sourceCoordinateZ }, _sourceIndex);
	_port  = new PortGaussian<float>("FixturePort" ,portLocation,sourceType,sourceAxis, "TestRun", 30 ,100);
}

