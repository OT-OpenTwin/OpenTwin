// @otlicense

#pragma once
#include "Simulation/FITTD3DLoop.h"
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include <memory>

class FITTD3DBuilder
{
private:
	Grid<float> * _grid = nullptr;
	CurlCoefficients3D<float> * _coefficients = nullptr;
	CurlCoefficients3D<float> * _coefficientsDual = nullptr;

	FITTD3DBuilder() {};
public:
	

	void AddGridSP(Grid<float> * grid) { _grid = grid; };
	void AddCurlCoefficientsSP(CurlCoefficients3D<float> * coefficients) { _coefficients= coefficients; };
	void AddCurlCoefficientsDualSP(CurlCoefficients3D<float> * coefficientsDual) { _coefficientsDual = coefficientsDual; };

	FITTD3DLoop<float>* CreateSimulationSP();

	static FITTD3DBuilder & GetInstance() 
	{
		static FITTD3DBuilder INSTANCE;
		return INSTANCE; 
	}
	//ToDo: Consistencycheck
};

static FITTD3DBuilder fittd3DBuilder = FITTD3DBuilder::GetInstance();
