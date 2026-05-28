// @otlicense
// File: ProblemTypeElectromagneticHF.cpp
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

#include "ProblemTypeElectromagneticHF.h"
#include "CartesianMeshMaterial.h"

#include <cmath>

double ProblemTypeElectromagneticHF::getBaseStepWidth() 
{ 
	if (maximumFrequencySI == 0.0) return 0.0;  
	
	return c0 / maximumFrequencySI / stepsPerWavelength / geomScaleFactor;
}

double ProblemTypeElectromagneticHF::getMaterialBaseStepWidth(CartesianMeshMaterial* material)
{
	double baseStep = getBaseStepWidth();

	return baseStep / std::sqrt(material->getEpsilon() * material->getMu());
}
