// @otlicense

#pragma once
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include <memory.h>
class Coefficient3DFactory
{
private:
	double relativePermittivityAir = 1.00059;
	double relativePermeabilityAir = 1 + 4 * pow(10, -7);
	Coefficient3DFactory() {};

public:

	static Coefficient3DFactory & GetInstance()
	{ 
		static Coefficient3DFactory INSTANCE;
		return INSTANCE;
	}
	CurlCoefficients3D<float>* CreateUniformAirCoefficientsSP(Grid<float> & grid);
	CurlCoefficients3D<float>* CreateUniformAirDualCoefficientsSP(Grid<float> & grid);
	CurlCoefficients3D<float>* CreateUniformPECCoefficientsSP(Grid<float> & grid);

};

static Coefficient3DFactory coefficient3DFactory = Coefficient3DFactory::GetInstance();