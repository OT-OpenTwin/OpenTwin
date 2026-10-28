// @otlicense
// File: VtkDriverFarfield.h
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

#include "VtkDriverWithScaling.h"

#include <string>
#include <ctime>
#include <complex>

class vtkStructuredGrid;
class DataSourceVtkFarfield;

class VtkDriverFarfield : public VtkDriverWithScaling
{
public:
	VtkDriverFarfield();
	virtual ~VtkDriverFarfield();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem, std::string& colorRamp) override;

private:
	double getFarfieldMaxRadius(vtkStructuredGrid* grid);
	void buildVtkGrid(DataSourceVtkFarfield* dataSource, vtkStructuredGrid* vtkGrid);
	inline double getPlotComponent(std::complex<double> &value);
	void determinePlotType(EntityVis2D3D* visEntity);

	enum plotType { MAGNITUDE, PHASE, REAL, IMAGINARY };

	double structureCx = 1.0;
	double structureCy = 1.0;
	double structureCz = 1.0;
	double structureD = 1.0;
	double scaleFactor = 1.0;
	bool   showEdges = true;
	plotType plotComponent = MAGNITUDE;
};
