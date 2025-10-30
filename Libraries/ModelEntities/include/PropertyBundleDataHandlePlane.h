// @otlicense
// File: PropertyBundleDataHandlePlane.h
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
#include "PlaneProperties.h"

class __declspec(dllexport) PropertyBundleDataHandlePlane : public PropertyBundleDataHandle
{
public:
	explicit PropertyBundleDataHandlePlane(EntityBase * thisObject);

	const double GetNormalValueX(void) const { return normalValueX; }
	const double GetNormalValueY(void) const { return normalValueY; }
	const double GetNormalValueZ(void) const { return normalValueZ; }
	const double GetCenterValueX(void) const { return centerX; }
	const double GetCenterValueY(void) const { return centerY; }
	const double GetCenterValueZ(void) const { return centerZ; }

	const PlaneProperties::NormalDescription GetNormalDescription(void)const { return normalDescription; }

private:
	double normalValueX;
	double normalValueY;
	double normalValueZ;
	double centerX;
	double centerY;
	double centerZ;
	PlaneProperties::NormalDescription normalDescription;
	
	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
