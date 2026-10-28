// @otlicense
// File: DataSourceVtkFarfield.cpp
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

#include "DataSourceVtkFarfield.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityResultFarfield.h"
#include "OTModelEntities/EntityAPI.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkTetra.h>

DataSourceVtkFarfield::DataSourceVtkFarfield()
{
}

DataSourceVtkFarfield::~DataSourceVtkFarfield()
{
	FreeMemory();
}

bool DataSourceVtkFarfield::loadData(EntityBase* resultEntity, EntityBase* meshEntity)
{
	EntityResultFarfield* farfield = dynamic_cast<EntityResultFarfield*>(resultEntity);

	if (farfield != nullptr)
	{
		return loadData(farfield);
	}

	assert(0);
	return false;
}

bool DataSourceVtkFarfield::loadData(EntityResultFarfield* farfield)
{
	FreeMemory();

	assert(farfield != nullptr);

	// Create test data <<<<<<<<<<<<<<<<<<<<<<<<< To be changed
	constexpr int thetaSteps = 60;
	constexpr int phiSteps = 120;

	const double pi = std::acos(-1.0);

	// including theta = 180 and phi = 360.
	theta = std::vector<double>(thetaSteps + 1);
	phi = std::vector<double>(phiSteps + 1);

	// create angle fields in degrees.
	for (int i = 0; i <= thetaSteps; ++i)
	{
		theta[i] = 180.0 * i / thetaSteps;
	}

	for (int j = 0; j <= phiSteps; ++j)
	{
		phi[j] = 360.0 * j / phiSteps;
	}

	// test function  <<<<<<<<< to be changed
	radius = std::vector<std::vector<std::complex<double>>>(thetaSteps + 1, std::vector<std::complex<double>>(phiSteps + 1));

	for (int i = 0; i <= thetaSteps; ++i)
	{
		const double t = theta[i] * pi / 180.0;
		const double sinT = std::sin(t);

		for (int j = 0; j < phiSteps; ++j)
		{
			const double p = phi[j] * pi / 180.0;

			// Same amplitude pattern as before
			const double amplitude =
				1.0
				+ 0.35 * sinT * sinT * std::cos(2.0 * p)
				+ 0.20 * std::cos(t);

			// Artificial test phase in radians.
			// Periodic in phi and independent of phi at the poles.
			const double phase = 0.75 * pi * sinT * std::cos(p);

			// amplitude * (cos(phase) + i * sin(phase))
			radius[i][j] = std::polar(amplitude, phase);
		}

		// Same complex value at phi = 360 and phi = 0
		radius[i][phiSteps] = radius[i][0];
	}

	return true;
}

void DataSourceVtkFarfield::FreeMemory(void)
{
	radius.clear();
	theta.clear();
	phi.clear();
}

