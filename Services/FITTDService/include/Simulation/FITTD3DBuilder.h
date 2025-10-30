// @otlicense
// File: FITTD3DBuilder.h
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
