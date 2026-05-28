// @otlicense
// File: ProblemTypeElectromagneticHF.h
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

#include "ProblemType.h"

class ProblemTypeElectromagneticHF : public ProblemType
{
public:
	ProblemTypeElectromagneticHF(double _maximumFrequencySI, double _stepsPerWavelength, double _geomScaleFactor)
		: maximumFrequencySI(_maximumFrequencySI), stepsPerWavelength(_stepsPerWavelength), geomScaleFactor(_geomScaleFactor)
	{
	};

	virtual ~ProblemTypeElectromagneticHF() {};

	virtual double getBaseStepWidth() override;
	virtual double getMaterialBaseStepWidth(CartesianMeshMaterial* material) override;

private:
	const double c0 = 299792458;

	double maximumFrequencySI = 0.0;
	double stepsPerWavelength = 0.0;
	double geomScaleFactor = 0.0;
};
