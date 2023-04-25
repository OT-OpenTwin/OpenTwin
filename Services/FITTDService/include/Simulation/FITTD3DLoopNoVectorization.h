#pragma once
#include "Simulation/FITTD3DLoop.h"

template<class T>
class FITTD3DLoopNoVectorization : public FITTD3DLoop<T>
{
private:
	using FITTD3DLoop<T>::_grid;
	using FITTD3DLoop<T>::_coefficients;
	using FITTD3DLoop<T>::_coefficientsDual;
	using FITTD3DLoop<T>::_doF;

public:
	FITTD3DLoopNoVectorization(Grid<T> *grid, CurlCoefficients3D<T> *coefficients, CurlCoefficients3D<T> *coefficientsDual)
		: FITTD3DLoop<T>(grid, coefficients, coefficientsDual) {};
	FITTD3DLoopNoVectorization(FITTD3DLoopNoVectorization&other) = delete;
	FITTD3DLoopNoVectorization & operator=(FITTD3DLoopNoVectorization &other) = delete;

	virtual void UpdateSimulationValuesE() override;
	virtual void UpdateSimulationValuesH() override;
};

#include "FITTD3DLoopNoVectorization.hpp"