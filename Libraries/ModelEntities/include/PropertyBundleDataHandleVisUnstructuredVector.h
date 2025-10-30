// @otlicense
// File: PropertyBundleDataHandleVisUnstructuredVector.h
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
#include "PropertiesVisUnstructuredVector.h"

class _declspec(dllexport) PropertyBundleDataHandleVisUnstructuredVector : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVisUnstructuredVector(EntityBase * thisObject);

	const PropertiesVisUnstructuredVector::VisualizationType GetSelectedVisType(void) const { return selectedVisType; }
	const PropertiesVisUnstructuredVector::VisualizationComponent GetSelectedVisComp(void) const {return selectedVisComp; }
	const int getMaxArrows(void) const { return maxArrows; }
	const PropertiesVisUnstructuredVector::VisualizationArrowType GetSelectedArrowType(void) { return selectedArrowType; }
	const double GetArrowScale(void) { return arrowScale; }
	const bool GetShow2dMesh(void) { return show2dMesh; }
	const bool GetShow2dIsolines(void) { return show2dIsolines; }
	const double GetColor2dMeshR(void) { return color2dMesh[0]; }
	const double GetColor2dMeshG(void) { return color2dMesh[1]; }
	const double GetColor2dMeshB(void) { return color2dMesh[2]; }
	const double GetColor2dIsolinesR(void) { return color2dIsolines[0]; }
	const double GetColor2dIsolinesG(void) { return color2dIsolines[1]; }
	const double GetColor2dIsolinesB(void) { return color2dIsolines[2]; }

private:
	PropertiesVisUnstructuredVector properties;
	PropertiesVisUnstructuredVector::VisualizationType selectedVisType;
	PropertiesVisUnstructuredVector::VisualizationComponent selectedVisComp;
	int maxArrows;
	PropertiesVisUnstructuredVector::VisualizationArrowType selectedArrowType;
	double arrowScale;
	bool show2dMesh;
	bool show2dIsolines;
	double color2dMesh[3];
	double color2dIsolines[3];

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
