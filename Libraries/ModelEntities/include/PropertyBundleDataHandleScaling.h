// @otlicense
// File: PropertyBundleDataHandleScaling.h
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
#include "PropertyBundleDataHandle.h"
#include "ScalingProperties.h"

class __declspec(dllexport) PropertyBundleDataHandleScaling : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleScaling(EntityBase * thisObject);

	const double GetRangeMin(void) const { return rangeMin; }
	const double GetRangeMax(void) const { return rangeMax; }
	const int GetColourResolution(void) const { return colourResolution; }
	const ScalingProperties::ScalingMethod GetScalingMethod(void) { return scalingMethod; }
	const ScalingProperties::ScalingFunction GetScalingFunction(void) { return scalingFunction; }

	bool UpdateMinMaxProperties(double minValue, double maxValue);
	void GetEntityProperties(long long& _UID, long long& Version);

private:
	EntityBase* thisObject = nullptr;

	ScalingProperties properties;
	ScalingProperties::ScalingFunction scalingFunction;
	ScalingProperties::ScalingMethod scalingMethod;
	double rangeMin=-1.;
	double rangeMax=-1.;
	int colourResolution;

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
