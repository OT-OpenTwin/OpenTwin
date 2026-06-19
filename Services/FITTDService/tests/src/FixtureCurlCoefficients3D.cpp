// @otlicense
// File: FixtureCurlCoefficients3D.cpp
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

#include "FixtureCurlCoefficients3D.h"
#include "Grid/Grid.h"
#include "SystemDependencies/Allignments.h"
#include"CurlCoefficients/Coefficient3DFactory.h"

void FixtureCurlCoefficients3D::SetCorrectUniformCubeGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF,CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformAirCoefficientsSP(*grid);
}

void FixtureCurlCoefficients3D::SetCorrectUniformCubeDualGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF, CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformAirDualCoefficientsSP(*grid);
}

FixtureCurlCoefficients3D::~FixtureCurlCoefficients3D()
{
	if (coefficients != nullptr) 
	{
		delete coefficients; 
	};
	if (grid != nullptr) 
	{ 
		delete grid; 
	};

}

void FixtureCurlCoefficients3D::SetCorrectUniformPECCubeGrid()
{
	index_t doF = 20;
	grid = new Grid<float>(doF, doF, doF, CacheLine64);
	coefficients = coefficient3DFactory.CreateUniformPECCoefficientsSP(*grid);
}
