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
