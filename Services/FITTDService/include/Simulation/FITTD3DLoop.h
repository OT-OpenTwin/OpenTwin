// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "Simulation/FITTD.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include "SystemDependencies/Allignments.h"

template <class T>
class FITTD3DLoop : public FITTD
{
protected:
	
	Grid<T> * _grid = nullptr;
	CurlCoefficients3D<T> * _coefficients = nullptr;
	CurlCoefficients3D<T> * _coefficientsDual = nullptr;

	DegreesOfFreedom3DLoopCPU<T> _doF;

public:
	explicit FITTD3DLoop( Grid<T> *grid , CurlCoefficients3D<T> *coefficients, CurlCoefficients3D<T> *coefficientsDual) 
		: _grid(grid), _coefficients(coefficients),_coefficientsDual(coefficientsDual), _doF(_grid->GetDegreesOfFreedomNumberTotalPadded(), _grid->GetAlignment()) {};

	FITTD3DLoop(FITTD3DLoop & other) = delete;
	FITTD3DLoop & operator=(FITTD3DLoop & other) = delete;
	virtual ~FITTD3DLoop() 
	{
		delete _grid;
		delete _coefficients;
		delete _coefficientsDual;
	}

	index_t GetIndexToCoordinate(index_t x, index_t y, index_t z) const { return _grid->GetIndexToCoordinate(x, y, z); };
	int GetVectorLength() const { return _grid->GetLengthOfIntrinsicVector(); };
	const Alignment GetAlignment() const { return _grid->GetAlignment(); };

	DegreesOfFreedom3DLoopCPU<T> * GetDegreesOfFreedom() { return &_doF; };
	const T GetTimeDiscretization() const { return _coefficients->GetTimeDiscretization();};
};

