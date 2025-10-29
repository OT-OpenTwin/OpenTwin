// @otlicense

#include "Simulation/FITTD3DBuilder.h"
#include "Simulation/FITTD3DLoopNoVectorization.h"
#include "Simulation/FITTD3DLoopAVX512Sp.h"
#include "Simulation/FITTD3DLoopAVX2.h"

FITTD3DLoop<float>* FITTD3DBuilder::CreateSimulationSP()
{
	if (_grid == nullptr || _coefficients == nullptr || _coefficientsDual == nullptr)
	{
		throw std::invalid_argument("FITTD3DBuilder has not all required components for assembling a FITTD3DLoop object.");
	}
	
	FITTD3DLoop<float>* solver;

	if (_grid->GetAlignment() == CacheLine64)
	{
		solver = (new FITTD3DLoopNoVectorization<float>(_grid, _coefficients,_coefficientsDual));
	}
	else if(_grid->GetAlignment() == AVX512)
	{
		solver = (new FITTD3DLoopAVX512Sp(_grid, _coefficients, _coefficientsDual));
	}
	else if (_grid->GetAlignment() == AVX2)
	{
		solver = (new FITTD3DLoopAVX2(_grid, _coefficients, _coefficientsDual));
	}
	else
	{
		throw std::invalid_argument("Selected alignment not supported.");
	}

	_grid = nullptr;
	_coefficients = nullptr;
	_coefficientsDual = nullptr;

	return solver;
}