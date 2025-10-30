// @otlicense
// File: FITTD3DBuilder.cpp
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