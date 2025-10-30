// @otlicense
// File: PropertyBundleDataHandleVis2D3D.h
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
#include "PropertiesVis2D3D.h"

class _declspec(dllexport) PropertyBundleDataHandleVis2D3D : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVis2D3D(EntityBase * thisObject);

	const PropertiesVis2D3D::VisualizationType GetSelectedVisType(void) const { return selectedVisType; }
	const PropertiesVis2D3D::VisualizationComponent GetSelectedVisComp(void) const {return selectedVisComp; }
	const int GetDownsamplingRate(void) const { return downsamplingRate; }

private:
	PropertiesVis2D3D properties;
	PropertiesVis2D3D::VisualizationType selectedVisType;
	PropertiesVis2D3D::VisualizationComponent selectedVisComp;
	int downsamplingRate;


	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
