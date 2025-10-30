// @otlicense
// File: Coefficient3DFactory.cpp
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

#include "CurlCoefficients/Coefficient3DFactory.h"
#include "Grid/Grid.h"
#include "Grid/EdgeDiscretization.h"
#include "Grid/SurfaceDiscretization.h"
#include "Grid/MaterialProperties.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include <vector>


CurlCoefficients3D<float>* Coefficient3DFactory::CreateUniformAirCoefficientsSP(Grid<float> & grid)
{	
	index_t combinedVectorDimension = grid.GetDegreesOfFreedomNumberTotal() * 3;

	double edgeDiscretizationValue = 0.5;
	std::vector<double> edgeDiscretizationVector(combinedVectorDimension, edgeDiscretizationValue);
	
	double surfaceDiscretizationValue = 0.25;
	std::vector<double> surfaceDiscretizationVector(combinedVectorDimension, surfaceDiscretizationValue);

	EdgeDiscretization eD;
	eD.ExtractComponentsFromSingleVector(edgeDiscretizationVector);

	SurfaceDiscretization sD;
	sD.ExtractComponentsFromSingleVector(surfaceDiscretizationVector);

	std::vector<double> permittivityVector(combinedVectorDimension, relativePermittivityAir);
	std::vector<double> permeabilityVector(combinedVectorDimension, relativePermeabilityAir);
	
	MaterialProperties mP(permeabilityVector.data(), permittivityVector.data(), grid.GetDegreesOfFreedomNumberTotal());

	auto coefficients = ( new CurlCoefficients3D<float>(grid));
	coefficients->SummarizeDescretizationAsCoefficients(sD, eD, mP, &MaterialProperties::GetPermeabilityInX, &MaterialProperties::GetPermeabilityInY, &MaterialProperties::GetPermeabilityInZ);

	return coefficients;
}

CurlCoefficients3D<float>* Coefficient3DFactory::CreateUniformAirDualCoefficientsSP(Grid<float> & grid)
{
	index_t combinedVectorDimension = grid.GetDegreesOfFreedomNumberTotal() * 3;

	double edgeDiscretizationValue = 0.5;
	std::vector<double> edgeDiscretizationVector(combinedVectorDimension, edgeDiscretizationValue);

	double surfaceDiscretizationValue = 0.25;
	std::vector<double> surfaceDiscretizationVector(combinedVectorDimension, surfaceDiscretizationValue);

	EdgeDiscretization eD;
	eD.ExtractComponentsFromSingleVector(edgeDiscretizationVector);

	SurfaceDiscretization sD;
	sD.ExtractComponentsFromSingleVector(surfaceDiscretizationVector);

	std::vector<double> permittivityVector(combinedVectorDimension, relativePermittivityAir);
	std::vector<double> permeabilityVector(combinedVectorDimension, relativePermeabilityAir);

	MaterialProperties mP(permeabilityVector.data(), permittivityVector.data(), grid.GetDegreesOfFreedomNumberTotal());
	
	CurlCoefficients3D<float>* coefficients ( new CurlCoefficients3D<float>(grid));
	coefficients->SummarizeDescretizationAsCoefficients(sD, eD, mP, &MaterialProperties::GetPermittivityInX, &MaterialProperties::GetPermittivityInY,  &MaterialProperties::GetPermittivityInZ);

	return coefficients;
}

CurlCoefficients3D<float>* Coefficient3DFactory::CreateUniformPECCoefficientsSP(Grid<float> & grid)
{
	index_t combinedVectorDimension = grid.GetDegreesOfFreedomNumberTotal() * 3;

	double edgeDiscretizationValue = 0.5;
	std::vector<double> edgeDiscretizationVector(combinedVectorDimension, edgeDiscretizationValue);

	double surfaceDiscretizationValue = 0.25;
	std::vector<double> surfaceDiscretizationVector(combinedVectorDimension, surfaceDiscretizationValue);

	EdgeDiscretization eD;
	eD.ExtractComponentsFromSingleVector(edgeDiscretizationVector);

	SurfaceDiscretization sD;
	sD.ExtractComponentsFromSingleVector(surfaceDiscretizationVector);

	std::vector<double> permittivityVector(combinedVectorDimension, 0.f);
	std::vector<double> permeabilityVector(combinedVectorDimension, 0.f);

	MaterialProperties mP(permeabilityVector.data(), permittivityVector.data(), grid.GetDegreesOfFreedomNumberTotal());
	
	CurlCoefficients3D<float>* coefficients ( new CurlCoefficients3D<float>(grid));
	coefficients->SummarizeDescretizationAsCoefficients(sD, eD, mP, &MaterialProperties::GetPermeabilityInX, &MaterialProperties::GetPermeabilityInY, &MaterialProperties::GetPermeabilityInZ);

	return coefficients;
}
