#include "FixtureCurlCoefficients3D.h"
#include "Grid/Grid.h"
#include "SystemDependencies/Allignments.h"
#include"CurlCoefficients/Coefficient3DFactory.h"

void FixtureCurlCoefficients3D::SetCorrectUniformCubeGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF,CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformAirCoefficientsSP(*grid);
}

void FixtureCurlCoefficients3D::SetCorrectUniformCubeDualGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF, CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformAirDualCoefficientsSP(*grid);
}

FixtureCurlCoefficients3D::~FixtureCurlCoefficients3D()
{
	if (coefficients != nullptr) 
	{
		delete coefficients; 
	};
	if (grid != nullptr) 
	{ 
		delete grid; 
	};

}

void FixtureCurlCoefficients3D::SetCorrectUniformPECCubeGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF, CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformPECCoefficientsSP(*grid);
}
