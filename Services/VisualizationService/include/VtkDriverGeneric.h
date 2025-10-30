// @otlicense
// File: VtkDriverGeneric.h
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

#include <string>
#include <ctime>

#include "VtkDriver.h"

class VtkDriverGeneric : public VtkDriver
{
public:
	VtkDriverGeneric();
	virtual ~VtkDriverGeneric();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;

private:
	enum tType {ARROWS, CONTOUR, CARPET, ISOLINES};

	void setVisualizationType(tType type) { visualizationType = type; }
	void setNormal(double nx, double ny, double nz) { normal[0] = nx; normal[1] = ny; normal[2] = nz; };
	void setCenter(double cx, double cy, double cz) { center[0] = cx; center[1] = cy; center[2] = cz; };

	std::string dataName;
	tType visualizationType;
	double normal[3];
	double center[3];
};
