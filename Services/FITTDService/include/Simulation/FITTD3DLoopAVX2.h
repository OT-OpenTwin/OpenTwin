// @otlicense
// File: FITTD3DLoopAVX2.h
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
#include "SystemDependencies/SystemDependentDefines.h"
#include "Simulation/FITTD3DLoop.h"
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"

class FITTD3DLoopAVX2 : public FITTD3DLoop<float>
{
private:
	using FITTD3DLoop<float>::_grid;
	using FITTD3DLoop<float>::_coefficients;
	using FITTD3DLoop<float>::_coefficientsDual;
	using FITTD3DLoop<float>::_doF;

	int* _maskBeginNoBeginningExclude;
	int* _maskBegin;
	int* _maskEnd;

	inline void UpdateEx(index_t index);
	inline void UpdateEy(index_t index);
	inline void UpdateEz(index_t index);
	inline void UpdateHx(index_t index);
	inline void UpdateHy(index_t index);
	inline void UpdateHz(index_t index);

	inline void MaskedUpdateEx(index_t index, int* mask);
	inline void MaskedUpdateEy(index_t index, int* mask);
	inline void MaskedUpdateEz(index_t index, int* mask);
	inline void MaskedUpdateHx(index_t index, int* mask);
	inline void MaskedUpdateHy(index_t index, int* mask);
	inline void MaskedUpdateHz(index_t index, int* mask);

public:
	FITTD3DLoopAVX2(Grid<float> *grid, CurlCoefficients3D<float> *coefficients, CurlCoefficients3D<float> *coefficientsDual);
	FITTD3DLoopAVX2(FITTD3DLoopAVX2 & other) = delete;
	FITTD3DLoopAVX2 & operator=(FITTD3DLoopAVX2 & other) = delete;

	virtual void UpdateSimulationValuesE() override;
	virtual void UpdateSimulationValuesH() override;
};

#include "Simulation/FITTD3DLoopAVX2.hpp"
