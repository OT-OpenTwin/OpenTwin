// @otlicense
// File: Coefficient3DFactory.h
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