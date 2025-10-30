// @otlicense
// File: FITTD3DLoopNoVectorization.h
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